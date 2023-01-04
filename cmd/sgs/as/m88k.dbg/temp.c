main()
{
	char c;
	printf("hello world\n");
	c = getchar();
	switch (c) {
		case '1':
			printf("got a '1'\n");
			break;
		case 'a':
			printf("got an 'a'\n");
			break;
		default:
			printf("invalid\n");
			break;
	}
}
