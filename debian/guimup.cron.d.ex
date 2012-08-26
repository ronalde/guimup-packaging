#
# Regular cron jobs for the guimup package
#
0 4	* * *	root	[ -x /usr/bin/guimup_maintenance ] && /usr/bin/guimup_maintenance
