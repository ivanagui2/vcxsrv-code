import os,sys

import sys

if len(sys.argv)>1 and sys.argv[1]=="1":
  os.system("cmd /c \"setenv.bat amd64 2> nul\"")
else:
  os.system("cmd /c \"setenv.bat x86 2> nul\"")

def readenv(filename):
  env={}
  fIN=open(filename, "r")
  for line in fIN.readlines():
    idx=line.find("=")
    if idx!=-1:
      var=line[:idx]
      val=line[idx+1:-1]
      env[var]=val
  return env

def escape(val):
  val=val.replace("\\","\\\\")
  return '"'+val+'"'

def escapepath(val):
  tmp={}
  paths=val.split(";")
  newpath=[]
  for path in paths:
    if not path in tmp:
      tmp[path]=1
      path=path.replace("c:","/cygdrive/c")
      path=path.replace("C:","/cygdrive/c")
      path=path.replace("\\","/")
      path=path.replace(" ","\\ ")
      path=path.replace("(","\\(")
      path=path.replace(")","\\)")
      newpath.append(path)
  return ":".join(newpath) 

env_before=readenv("env_before.txt")
env_after=readenv("env_after.txt")
os.remove("env_before.txt")
os.remove("env_after.txt")

for var,val in env_after.items():
  if not var in env_before:
    print("export %s=%s"%(var,escape(val)))
  else:
    oldval=env_before[var]
    if val != oldval:
      if var!="PATH":
        pass
        # currently only allow path to be different
        #print var,"different"
        #print oldval
        #print val
      else:
        print("export PATH=%s"%(escapepath(val)))

