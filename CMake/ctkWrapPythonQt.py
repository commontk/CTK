
import errno
import os
import re
from string import Template

PYTHONQT_WRAPPER_WITH_PARENT = Template("""
//-----------------------------------------------------------------------------
class PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public Q_SLOTS:
  ${className}* new_${className}(${parentClassName}*  parent = 0)
    {
    return new ${className}(parent);
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
""")

PYTHONQT_WRAPPER_WITHOUT_PARENT = Template("""
//-----------------------------------------------------------------------------
class PythonQtWrapper_${className} : public QObject
{
Q_OBJECT
public:
public Q_SLOTS:
  ${className}* new_${className}()
    {
    return new ${className}();
    }
  void delete_${className}(${className}* obj) { delete obj; }
};
""")

def _mkdir_p(path):
    """See """
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

def ctk_wrap_pythonqt(target, namespace, output_dir, input_files, extra_verbose):
    if extra_verbose:
        print("target: %s" % target)
        print("namespace: %s" % namespace)
        print("output_dir: %s" % output_dir)
        print("input_files: %s" % input_files)
    
    _mkdir_p(output_dir)
    
    includes = []
    pythonqtWrappers = []
    registerclasses = []
    namespace = namespace.replace('.', '_')
    
    for input_file in input_files:
        filename = os.path.basename(input_file)
        if extra_verbose:
            print("Wrapping %s" % filename)
       
        # what is the filename without the extension
        filename_we = os.path.splitext(filename)[0]

        # Extract classname - NOTE: We assume the filename matches the associated class
        className = filename_we
        if extra_verbose:
            print("\tclassName:%s" % className)

        # Extract parent classname
        parentClassName = None

        # Read input files
        with open(input_file) as f:
            content = f.read()
            
            # Skip wrapping if file do NOT contain Q_OBJECT
            if 'Q_OBJECT' not in content:
                if extra_verbose:
                    print("\tskipping - No Q_OBJECT macro")
                continue

            # Skip wrapping if constructor doesn't match:
            #    my_class()
            #    my_class(QObject* newParent ...)
            #    my_class(QWidget* newParent ...)
            # Constructor with either QWidget or QObject as first parameter
            regex = r"[^~]%s[\s\n]*\([\s\n]*((QObject|QWidget)[\s\n]*\*[\s\n]*\w+[\s\n]*(\=[\s\n]*(0|NULL)|,.*\=.*\)|\)|\)))" % className
            res = re.search(regex, content, re.MULTILINE)
            if res is None:
                if extra_verbose:
                    print("\tskipping - Missing expected constructor signature")
                continue
     
            # Skip wrapping if object has a virtual pure method
            # "x3b" is the unicode for semicolon
            regex = r"virtual[\w\n\s\*\(\)]+\=[\s\n]*(0|NULL)[\s\n]*\x3b"
            res = re.search(regex, content, re.MULTILINE)
            if res is not None:
                if extra_verbose:
                    print("skipping - Contains a virtual pure method")
                continue

            if parentClassName is None:
                # Does constructor signature is of the form: myclass()
                regex = r"[^~]%s[\s\n]*\([\s\n]*\)" % className
                res = re.search(regex, content, re.MULTILINE)
           
                if res is not None:
                    parentClassName = ""
                    if extra_verbose:
                        print("\tconstructor of the form: %s()" % className)

            if parentClassName is None:
                # Does constructor signature is of the form: myclass(QObject * parent ...)
                regex = r"%s[\s\n]*\([\s\n]*QObject[\s\n]*\*[\s\n]*\w+[\s\n]*(\=[\s\n]*(0|NULL)|,.*\=.*\)|\))" % className
                res = re.search(regex, content, re.MULTILINE)
                if res is not None:
                    parentClassName = "QObject"
                    if extra_verbose:
                        print("\tconstructor of the form: %s(QObject * parent ... )" % className)

            if parentClassName is None:
                # Does constructor signature is of the form: myclass(QWidget * parent ...)
                regex = r"%s[\s\n]*\([\s\n]*QWidget[\s\n]*\*[\s\n]*\w+[\s\n]*(\=[\s\n]*(0|NULL)|,.*\=.*\)|\))" % className
                res = re.search(regex, content, re.MULTILINE)
                if res is not None:
                    parentClassName = "QWidget"
                    if extra_verbose:
                        print("\tconstructor of the form: %s(QWidget * parent ... )" % className)

        if parentClassName is not None:
            includes.append('#include "%s.h"' % filename_we)

        # Generate PythonQtWrapper class
        if parentClassName == "QObject" or parentClassName == "QWidget":
            pythonqtWrappers.append(
                PYTHONQT_WRAPPER_WITH_PARENT.substitute(className = className, parentClassName = parentClassName))
            
        elif parentClassName == "":
            pythonqtWrappers.append(PYTHONQT_WRAPPER_WITHOUT_PARENT.substitute(className = className))

        else: # Case parentClassName is None
            raise Exception("Problem wrapping %s" % input_file)
        
        # Generate code allowing to register the class metaobject and its associated "light" wrapper
        registerclasses.append(
        Template("""
  PythonQt::self()->registerClass(
  &${className}::staticMetaObject, "${target}",
  PythonQtCreateObject<PythonQtWrapper_${className}>);
  """).substitute(className = className, target = target))

    output_header = output_dir + "/" + namespace + "_" + target + ".h"
    if extra_verbose:
        print("output_header: %s" % output_header)
    # Write master include file
    with open(output_header, "w") as f:
        f.write(Template(
"""
//
// File auto-generated by ctkWrapPythonQt.py
//

#ifndef __${namespace}_${target}_h
#define __${namespace}_${target}_h

#include <QWidget>
${includes}
${pythonqtWrappers}
#endif
""").substitute(namespace = namespace, target = target, includes = '\n'.join(includes), pythonqtWrappers = '\n'.join(pythonqtWrappers)))

    output_cpp = output_dir + "/" + namespace + "_" + target + "_init.cpp"
    if extra_verbose:
        print("output_cpp: %s" % output_cpp)
    with open(output_cpp , "w") as f:
        # Write wrapper header
        f.write(Template(
"""
//
// File auto-generated by ctkWrapPythonQt.py
//

#include <PythonQt.h>
// XXX Avoid  warning: "HAVE_STAT" redefined
#undef HAVE_STAT
#include "${namespace}_${target}.h"

void PythonQt_init_${namespace}_${target}(PyObject* module)
{
  Q_UNUSED(module);
  ${registerclasses}
}
""").substitute(namespace = namespace, target = target, registerclasses = '\n'.join(registerclasses)))

if __name__ == '__main__':
    from optparse import OptionParser
    usage = "usage: %prog [options] <output_file> <input_file> [<input_file1> [...]]"
    parser = OptionParser(usage=usage)
    parser.add_option("-t", "--target",
                      dest="target", action="store", type="string",
                      help="Name of the associated library")
    parser.add_option("-n", "--namespace",
                      dest="namespace", action="store", type="string",
                      help="Wrapping namespace")
    parser.add_option("--output-dir",
                      dest="output_dir", action="store", type="string",
                      help="Output directory")
    parser.add_option("-v", "--verbose",
                      dest="verbose", action="store_true",
                      help="Print verbose information")
    parser.add_option("--extra-verbose",
                      dest="extra_verbose", action="store_true",
                      help="Print extra verbose information")

    (options, args) = parser.parse_args()

    #if len(args) < 2:
    #    parser.error("arguments '%s' are required !" % '<output_file> <input_file>')

    if options.extra_verbose:
        options.verbose = True

    ctk_wrap_pythonqt(options.target, options.namespace, options.output_dir, args, options.extra_verbose)
      
    if options.verbose:
        print("Wrapped %d files" % len(args))
