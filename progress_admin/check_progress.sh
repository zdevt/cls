#!/bin/sh
#       FileName:  check_progress.sh
#
#    Description:
#
#        Version:  1.0
#        Created:  2018-03-07 11:37:09
#  Last Modified:  2018-03-28 16:26:04
#       Revision:  none
#       Compiler:  gcc
#
#         Author:  zt ()
#   Organization:


  sta=`ps -fe | grep $1 | grep -v "grep" | awk '{print $2}'`

  #echo $sta
  if [ $sta -gt 0 ]
  then 
    echo "running"
  else 
    echo "error "
  fi


  # TODO ....







