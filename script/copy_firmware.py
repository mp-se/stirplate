# 
# MIT License
# 
# Copyright (c) 2021 Magnus
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

Import("env")
import shutil

def get_build_flag_value(flag_name):
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags_with_value_list = [build_flag for build_flag in build_flags.get('CPPDEFINES') if type(build_flag) == list]
    defines = {k: v for (k, v) in flags_with_value_list}
    return defines.get(flag_name)

def after_build(source, target, env):
    print( "Executing custom step " )
    dir    = env.GetLaunchDir()
    name   = env.get( "PIOENV" )
    source = dir + "/.pio/build/" + name + "/firmware.bin"
    if name == "stirplate-release-nowifi" : 
        target = dir + "/bin/firmware-nowifi.bin"
    if name == "stirplate-debug" :
        target = dir + "/bin/firmware-debug.bin"
    if name == "stirplate-release" :
        target = dir + "/bin/firmware.bin"
    print( "Copy file : " + source + " -> " + target )
    shutil.copyfile( source, target )

    target = dir + "/bin/version.json"
    ver = get_build_flag_value("CFG_APPVER")
    print( "Creating version.json" )
    f = open( target, "w" )
    f.write( "{ \"project\":\"stirplate\", \"version\":" + ver + " }" ) 
    f.close()


print( "Adding custom build step: ")
#env.AddPreAction("buildprog", after_build)
env.AddPostAction("buildprog", after_build)
#env.AddPreAction("upload", after_build)   
#env.AddPostAction("upload", after_build)
