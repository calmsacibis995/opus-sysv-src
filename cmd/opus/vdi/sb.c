#include <stdio.h>

#define NUMBER_OVHUES	16

main()
{
	int number, hue, i, j, k, l;
	static short openin[19] = {1,1,1,1,1,1,1,1,1,1,0,
		'D','I','S','P','L','A','Y' };
	static short openout[67];
	static short dev_handle;
	static short xy[4];
	char buf[32800];

	number=10;
	hue=1;
	vdi_open(0,buf,8000,0);
	v_opnwk(openin, &dev_handle, openout);
	for (i=0; i<NUMBER_OVHUES; i++) {
		if ((++hue) >= NUMBER_OVHUES)
			hue=1;
		vsf_color(dev_handle, hue);
		for (j=50; j<24630; j+=819) {
			for (k=50; k<24630; k+=819) {
				xy[0] = j;
				xy[1] = k;
				xy[2] = j + 716;
				xy[3] = k + 716;
				v_bar(dev_handle, xy);
			}
		}
	}
	v_clswk(dev_handle);
	vdi_close();
}
