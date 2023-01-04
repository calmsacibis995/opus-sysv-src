
#include <stdio.h>
#include <locale.h>

static char *locales[] = {
	"LC_CTYPE",		/* 0 */
	"LC_COLLATE",		/* 1 */
	"LC_TIME",		/* 2 */
	"LC_NUMERIC",		/* 3 */
	"LC_MONETARY",		/* 4 */
};

#define STRSIZE 1024

extern char *_getenv();
extern char *_strdup(), *_strcpy(), *_strcat(), *_strncat(), *_strchr();
extern char *_malloc();
extern int _putenv();

char *
_setlocale(category, locale)
int category;
char *locale;
{
	int i;
	char *envloc;
	char str[STRSIZE];
	static char *mstr;

	if (locale == NULL) {	/* getlocale */
		switch(category) {

		default:
			return NULL;

		case LC_CTYPE:
		case LC_COLLATE:
		case LC_TIME:
		case LC_NUMERIC:
		case LC_MONETARY:
			envloc = _getenv(locales[category]);
			if (envloc == NULL || envloc[0] == '\0') {
				envloc = _getenv("LANG");
				if (envloc == NULL || envloc[0] == '\0')
					envloc = "C";
			}
			return envloc;

		case LC_ALL:
			if (mstr == NULL) {
				mstr = _malloc(STRSIZE);
				if (mstr == NULL)
					return NULL;
			}
			mstr[0] = '\0';
			for (i=LC_CTYPE; i<=LC_MONETARY; ++i) {
				_strncat(mstr, _setlocale(i, NULL, STRSIZE/6));
				if (i != LC_MONETARY)
					_strcat(mstr, " ");
			}
			return mstr;
		}
	} else {	/* setlocale */

		switch (category) {

		default:
			return NULL;

		case LC_CTYPE:
		case LC_COLLATE:
		case LC_TIME:
		case LC_NUMERIC:
		case LC_MONETARY:
			if (locale[0] == '\0') {
				locale = _getenv(locales[category]);
				if (locale == NULL || locale[0] == '\0')
					locale = "C";
			}
			_strcpy(str, locales[category]);
			_strcat(str, "=");
			_strncat(str, locale, STRSIZE-32);
			_putenv(locale = _strdup(str));
			return _strchr(locale, '=')+1;

		case LC_ALL:
			for (i=LC_CTYPE; i<=LC_MONETARY; ++i)
				if ((envloc = _setlocale(i, locale)) == NULL)
					return NULL;
			return envloc;
		}
	}
}
