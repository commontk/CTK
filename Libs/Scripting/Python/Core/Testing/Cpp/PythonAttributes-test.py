from __future__ import print_function

class Maths(object):

  MATHS_CLASS_MEMBER=0.1

  def __init__(self, num):
    self.maths_instance_member = num

  def maths_instance_method(self):
    print("Hello from instance method")

class MultipleArg(object):
  def __init__(self, num, str, other = 0):
    self.multipleArg_instance_member_num = num + other
    self.multipleArg_instance_member_str = str
    self.multipleArg_instance_member_other = other

class Bar(object):

  BAR_CLASS_MEMBER = 1

  def __init__(self):
    self.bar_instance_member = 1

  def bar_instance_method(self):
    print("Hello from instance method")

  def bar_maths(self, num = 0):
    return Maths(num)

  @staticmethod
  def bar_class_method():
    print("Hello from class method")

class Foo(object):

  FOO_CLASS_MEMBER = 1

  def __init__(self):
    self.foo_instance_member = 1

  def foo_instance_method(self):
    print("Hello from instance method")

  def instantiate_bar(self):
    return Bar()

  @staticmethod
  def foo_class_method():
    print("Hello from class method")

f = Foo()

class Object(object): pass

d = Object()
setattr(d, 'foo_class', Foo)
