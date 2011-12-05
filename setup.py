from distutils.core import setup, Extension

mmseg = Extension('mmseg',
                  include_dirs = ['/opt/local/include/mmseg'],
                  libraries = ['mmseg'],
                  library_dirs = ['/opt/local/lib'],
                  sources = ['mmseg.cpp'])

setup (name = 'python-mmseg',
       version = '1.0',
       description = 'LibMMSeg binding for python',
       author = 'bearice@gmail.com',
       url = 'http://github.com/bearice/python-mmseg',
       ext_modules = [mmseg])

