main(argc, argv, envp)
char *argv[];
char *envp[];
{
	char *fname;
	char buff[1024];
	int retval;

	fname = argv[argc - 1];
	sprintf(buff, "/bin/sifilter -m %s /tmp/sifilt%d", fname, getpid());
	if (retval = system(buff)) 
		exit(retval);
	sprintf(buff, "/bin/mv /tmp/sifilt%d %s", getpid(), fname);
	if (retval = system(buff)) 
		exit(retval);
	execve("/lib/as", argv, envp);
	perror("/bin/as: execve");
	exit(1);
}
