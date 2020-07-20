```python
def init_internal_ip_pool(context):
    array_db = repository.ArrayIPPoolsRepository()
    empty = array_db.is_empty(context.session)
    if not empty:
        LOG.debug("array_ip_pool already exists and will not create it again")
        return
    try:
        #IPV6
        inter_ipv6 = cfg.CONF.arraynetworks.internal_ipv6_netlist
        max_inter_ipv6 = cfg.CONF.arraynetworks.max_subnet

        for num in range(0, int(max_inter_ipv6)):
            #for nat
            if inter_ipv6:
                internal_ipv6_for_nat = inter_ipv6[:inter_ipv6.index(":")] + "::" + str(num)
            else:
                internal_ipv6_for_nat = "1234::" + str(num)
            array_db.create(context.session, inter_ip=internal_ipv6_for_nat, used=False, ipv4=False, use_for_nat=True)

            #not for nat
            if num == 0:
                continue
            if inter_ipv6:
                internal_ipv6 = inter_ipv6[:inter_ipv6.index(":") + 1] + str(num) + "::0"
            else:
                internal_ipv6 = "1234:" + str(num) + "::0"
            array_db.create(context.session, inter_ip=internal_ipv6, used=False, ipv4=False, use_for_nat=False)
    except Exception as e:
        LOG.error("Failed to init array ip pool(%s)", e.message)

def init_internal_ip_pool_frag(context):
    array_db = repository.ArrayIPPoolFragRepository()
    empty = array_db.is_empty(context.session)
    if not empty:
        LOG.debug("array_ip_pool_frag already exists and will not create it again")
        return
    inter_ipv4_netlist = cfg.CONF.arraynetworks.internal_ipv4_netlist.split(',')
    try:
        ipv4_head_list = []
        for inter_ip in inter_ipv4_netlist:
            inter_ip = inter_ip.strip()
            ipv4_head_list.append(int(inter_ip[:inter_ip.index(".")]))
        ipv4_head_list.sort()
        def store_nets(iplist, start, end):
            if start == end:
                frag_start = str(iplist[start]) + ".0.0.0"
                frag_end = str(iplist[end]) + ".255.255.255"
                array_db.create(context.session, start=frag_start, end=frag_end, ipv4=True)
                return
            #find the first start and end
            ip_start = iplist[start]
            ip_end = iplist[end]
            last = False
            for i in range(start + 1, end + 1):
                if iplist[i] == (iplist[i-1] + 1):
                    if i == end:
                        last = True
                    continue
                else:
                    ip_end = iplist[i-1]
                    break
            #store the entry
            frag_start = str(ip_start) + ".0.0.0"
            frag_end = str(ip_end) + ".255.255.255"
            array_db.create(context.session, start=frag_start, end=frag_end, ipv4=True)
            if last:
                return
            #find remaining  start and end
            store_nets(iplist, i, end)
        store_nets(ipv4_head_list, 0, len(ipv4_head_list) - 1)
    except Exception as e:
        LOG.error("Failed to init array ip pool frag(%s)", e.message)

int2ip = lambda x: '.'.join([str(x/(256**i) % 256) for i in range(3, -1, -1)])
ip2int = lambda x: sum([256**i*int(j) for i, j in enumerate(x.split('.')[::-1])])

def gen_ip_net(netmask, ip):
    ip_int = ip2int(ip)
    mask_int = 1 << (32 - netmask)
    remain = ip_int % mask_int
    if remain == 0:
        ip_net_start = ip
        ip_net_end = int2ip(ip_int + mask_int - 1)
    else:
        start = ip_int + mask_int - remain
        end = start + mask_int - 1
        ip_net_start = int2ip(start)
        ip_net_end = int2ip(end)
    return ip_net_start, ip_net_end

def get_next_ip(ip):
    return int2ip(ip2int(ip) + 1)

def get_last_ip(ip):
    return int2ip(ip2int(ip) - 1)

def compare_ip(ip1, ip2):
    if hasattr(ip1, "start"):
        ip1 = ip1.start
    if hasattr(ip2, "start"):
        ip2 = ip2.start
    ip1s = ip1.split(".")
    ip2s = ip2.split(".")
    for i in range(0, 4):
        if int(ip1s[i]) > int(ip2s[i]):
            return 1
        elif int(ip1s[i]) < int(ip2s[i]):
            return -1
        else:
            continue
    return 0

def allocate_internal_ipv4(context, netmask):
    """
    Function allocate_internal_ipv4 will apply one internal ip according to db
    array_ip_pool_frag and update the db, it will be from 4 cases:
    before apply
      ip_net
    |_______|
    start  end

       frag1                 frag2                 last_frag
    |_______________|      |_______|     .....   |___________|
    start          end    start   end          start       end
    ----------------------------------------------------------
    after apply
    suppose the first frag can be satisfied, otherwise find from the next frag
    case1:
       alloc    frag1          frag2                 last_frag
    |.......|________|      |_______|     .....   |___________|
            start  end    start   end           start       end

    case2:
           alloc              frag2                 last_frag
    |................|      |_______|     .....   |___________|
                           start   end           start       end

    case3:
      frag1    alloc         frag2                 last_frag
    |_______|........|      |_______|     .....   |___________|
    start  end             start   end           start       end

    case4:
     frag1  alloc frag2        frag3                last_frag
    |_____|.....|____|      |_______|     .....   |___________|
    start  end start end   start   end           start       end
    """
    try:
        with context.session.begin(subtransactions=True):
            array_db = repository.ArrayIPPoolFragRepository()
            all_frag = array_db.get_all_frag(context.session)
            if not all_frag:
                LOG.debug("Failed to get internal ip frag data")
                return None
            all_frag.sort(cmp=compare_ip)
            allocate_internal_ip = None
            for one_frag in all_frag:
                ip_net_start, ip_net_end = gen_ip_net(netmask, one_frag.start)
                if compare_ip(ip_net_end, one_frag.end) > 0:
                    continue
                else:
                    LOG.debug("Got the internal ip(%s) by netmask %d", ip_net_start, netmask)
                    allocate_internal_ip = ip_net_start
                    if ip_net_start == one_frag.start:
                        frag_start = get_next_ip(ip_net_end)
                        frag_end = one_frag.end
                        if compare_ip(frag_start, frag_end) > 0:
                            #case2
                            array_db.delete(context.session, start=one_frag.start)
                        else:
                            #case1
                            array_db.update(context.session, one_frag.id, start=frag_start, end=frag_end, ipv4=True)
                    else:
                        frag1_start = one_frag.start
                        frag1_end = get_last_ip(ip_net_start)
                        frag2_start = get_next_ip(ip_net_end)
                        frag2_end = one_frag.end
                        #case3
                        array_db.update(context.session, one_frag.id, start=frag1_start, end=frag1_end, ipv4=True)
                        if compare_ip(frag2_end, frag2_start) >= 0:
                            #case4
                            array_db.create(context.session, start=frag2_start, end=frag2_end, ipv4=True)
                    break
            return allocate_internal_ip
    except Exception as e:
        LOG.ERROR("Failed to allocate internal ipv4 (%s)", e.message)

def release_internal_ipv4(context, ip, netmask):
    """
    Function release_internal_ipv4 will merge the need released internal ip
    to current multiple internal ip pool fragment as following 5 cases:

    before release:
       ip_net
    |_________|
    start    end

       frag1           frag2                 last_frag
    |_________|      |_______|     .....   |___________|
    start     end    start   end          start       end
    ----------------------------------------------------
    after release:
    case1: (ip_net_end + 1 < frag1_start)
       ip_net          frag1           frag2                 last_frag
    |_________|     |_________|      |_______|     .....   |___________|
       ip_net_end frag1_start       start   end          start       end

    case2: (ip_net_end + 1 = frag1_start)
       ip_net    frag1          frag2                 last_frag
    |__________________|      |_______|     .....   |___________|
    ip_net_start frag1_end   start   end          start       end

    case3: (frag1_end + 1 = ip_net_start < frag2_start - 1)
       frag1    ip_net          frag2                 last_frag
    |__________________|      |_______|     .....   |___________|
    frag1_start  ip_net_end  start   end          start       end

    case4: (frag1_end + 1 = ip_net_start = frag2_start - 1)
       frag1    ip_net   frag2                 last_frag
    |_________________________|     .....   |___________|
    frag1_start          frag2_end        start       end

    case5: (ip_net_start > last_frag_end + 1)
       frag1           frag2                 last_frag        ip_net
    |_________|      |_______|     .....   |___________|   |_________|
    start     end    start   end          start       end start    end
    """
    try:
        with context.session.begin(subtransactions=True):
            array_db = repository.ArrayIPPoolFragRepository()
            all_frag = array_db.get_all_frag(context.session)
            if not all_frag:
                ip_net_start, ip_net_end = gen_ip_net(netmask, ip)
                LOG.debug("Failed to get internal ip frag data by ip(%s) and netmask(%d),"
                            "the frag(%s,%s) will be created", ip, netmask, ip_net_start, ip_net_end)
                array_db.create(context.session, start=ip_net_start, end=ip_net_end, ipv4=True)
                return
            all_frag.sort(cmp=compare_ip)
            ip_net_start = ip
            mask_int = 1 << (32 - netmask)
            ip_net_end = int2ip(ip2int(ip) + mask_int - 1)
            found = False
            for idx, one_frag in enumerate(all_frag):
                if compare_ip(ip_net_start, get_next_ip(one_frag.end)) > 0:
                    continue
                else:
                    found = True
                    LOG.debug("Find the match frag start(%s), end(%s)", one_frag.start, one_frag.end)
                    if compare_ip(ip_net_end, get_last_ip(one_frag.start)) < 0:
                        #case1
                        array_db.create(context.session, start=ip_net_start, end=ip_net_end, ipv4=True)
                    elif compare_ip(ip_net_end, get_last_ip(one_frag.start)) == 0:
                        #case2
                        array_db.update(context.session, one_frag.id, start=ip_net_start, end=one_frag.end, ipv4=True)
                    else:
                        frag_start = one_frag.start
                        frag_end = ip_net_end
                        if idx < len(all_frag) - 1:
                            next_frag = all_frag[idx+1]
                            if compare_ip(get_last_ip(next_frag.start), ip_net_end) == 0:
                                frag_end = next_frag.end
                                #case4
                                array_db.delete(context.session, start=next_frag.start)
                        #case3
                        array_db.update(context.session, one_frag.id, start=frag_start, end=frag_end, ipv4=True)
                    break
            #final one
            if not found:
                #case5
                array_db.create(context.session, start=ip_net_start, end=ip_net_end, ipv4=True)
    except Exception as e:
        LOG.ERROR("Failed to release internal ipv4 (%s)", e.message)
```

