/*
  This code is derived from PDPCLIB, the public domain C runtime
  library by Paul Edwards. http://pdos.sourceforge.net/

  This code is released to the public domain.
*/

time_t pmktime (PblTm *tmptr);
PblTm *pgmtime (const time_t *timer);
char *pstrtok(char *s1, const char *s2);