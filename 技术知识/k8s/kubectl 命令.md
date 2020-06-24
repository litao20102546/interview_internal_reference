* kubectl get pod --show-tables
* kubectl cluster-info
* kubectl get pod -L env, app
* kubectl label pods/pod1 env=production (--overwrite)
* kubectl annotate pods pod1 "...."
* kubectl cp /tmp/file pod1:/tmp/
* kubectl top
* kubectl rollout status deployments <name>
* kubectl scale