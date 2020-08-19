import tensorflow as tf
tf.compat.v1.disable_eager_execution()
sess = tf.compat.v1.Session()

# constant multiply
x1=tf.constant(1)
y1=tf.constant(2)
#注意这里x,y必须要有相同的数据类型，不然就会因为数据类型不匹配报错
z1=tf.multiply(x1,y1)
print(z1)
print(sess.run(z1))

# one row two column +
a = tf.constant([1.0, 2.0])
b = tf.constant([3.0, 4.0])
result = a + b
print(result)
print(sess.run(result))

# matmul
a1 = tf.constant([[3.0, 4.0]])
a2 = tf.constant([[1.0], [2.0]])
result1 = tf.matmul(a1, a2)
print(result1)
print(sess.run(result1))

x=tf.constant([[1.,2.,3.],[1.,2.,3.],[1.,2.,3.]])
y=tf.constant([[0.,0.,1.],[0.,0.,1.],[0.,0.,1.]])
#注意这里x,y必须要有相同的数据类型，不然就会因为数据类型不匹配报错
z=tf.multiply(x,y)
print(z)
print(sess.run(z))
