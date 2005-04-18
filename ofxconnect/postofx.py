#!/usr/bin/python
import urllib2
import sys

argv = sys.argv
if __name__=="__main__":
  if len(argv) < 2:
    print "Usage:",sys.argv[0], "url"
    sys.exit()
  request = urllib2.Request(sys.argv[1],sys.stdin.read(),
                            { "Content-type": "application/x-ofx",
                              "Accept": "*/*, application/x-ofx"
                            })
  f = urllib2.urlopen(request)
  response = f.read()
  f.close()
  print response
  