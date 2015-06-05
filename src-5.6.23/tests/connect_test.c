/* Copyright (c) 2000, 2002-2004 MySQL AB
   Use is subject to license terms

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"

static void change_user(MYSQL *sock,const char *user, const char *password,
			const char *db,my_bool warning)
{
  if (mysql_change_user(sock,user,password,db) != warning)
  {
    fprintf(stderr,"Couldn't change user to: user: '%s', password: '%s', db: '%s':  Error: %s\n",
	    user, password ? password : "", db ? db : "",
	    mysql_error(sock));
  }
}


int main(int argc, char **argv)
{
  MYSQL *sock;

  if (!(sock=mysql_init(0)))//返回一个链接句柄
  {
    fprintf(stderr,"Couldn't initialize mysql struct\n");
    exit(1);
  }
      /*
       * int mysql_options(MYSQL *mysql, enum mysql_option option, const void *arg);
       *可用于设置额外的连接选项，并影响连接的行为。可多次调用该函数来设置数个选项。
       *应在mysql_init()之后、以及mysql_connect()或mysql_real_connect()之前调用mysql_options()。
       *选项参量指的是你打算设置的选项。Arg参量是选项的值。如果选项是整数，那么arg应指向整数的值。
       *注意，如果使用了MYSQL_READ_DEFAULT_FILE或MYSQL_READ_DEFAULT_GROUP，总会读取客户端组.
       */
  mysql_options(sock,MYSQL_READ_DEFAULT_GROUP,"connect");

  //尝试与运行在主机上的MySQL数据库引擎建立连接
  if (!mysql_real_connect(sock,NULL,NULL,NULL,NULL,0,NULL,0))
  {
    fprintf(stderr,"Couldn't connect to engine!\n%s\n",mysql_error(sock));
    perror("");
    exit(1);
  }
  sock->reconnect= 1;

  // 设定与指定的连接标识符所关联的服务器上的当前激活数据库
  if (mysql_select_db(sock,"test"))
  {
    fprintf(stderr,"Couldn't select database test: Error: %s\n",
	    mysql_error(sock));
  }

  change_user(sock,"test_user","test_user","test",0);
  change_user(sock,"test",NULL,"test",0);
  change_user(sock,"test_user",NULL,"test",1);
  change_user(sock,"test_user",NULL,NULL,1);
  change_user(sock,"test_user","test_user","mysql",1);

  mysql_close(sock);//关闭指定的连接标识所关联的到 MySQL 服务器的非持久连接
  exit(0);
  return 0;
}
