/*
 * krbauth.c: Check a username/password pair against krb5 by temporarily
 *            obtaining a short-lived ticket for the corresponding
 *            principal. Returns 1 in case of successful verification,
 *            0 otherwise.
 *
 * Nickolai Zeldovich <kolya@MIT.EDU>
 *
 */

#include <stddef.h>
#include <krb5.h>
#include <com_err.h>
#include <string.h>

#define KRB5_LIFETIME 5

int krbauth(char *user, char *pass) {

 krb5_data tgtname = { 0, KRB5_TGS_NAME_SIZE, KRB5_TGS_NAME };
 krb5_context kcontext;
 krb5_error_code code;
 krb5_principal me;
 krb5_principal server;
 krb5_creds my_creds;
 krb5_timestamp now;
 char *client_name;

 if((code=krb5_init_context (&kcontext))) {
#ifdef DEBUG
  com_err("krbauth", code, "while initializing krb5");
#endif
  return 0; }

 if((code=krb5_timeofday(kcontext,&now))) {
#ifdef DEBUG
  com_err("krbauth", code, "while getting time of day");
#endif
  return 0; }

 if((code=krb5_parse_name(kcontext, user, &me))) {
#ifdef DEBUG
  com_err("krbauth", code, "while parsing name %s", user);
#endif
  return 0; }

 if((code=krb5_unparse_name(kcontext, me, &client_name))) {
#ifdef DEBUG
  com_err("krbauth", code, "when unparsing name");
#endif
  return 0; }

 memset((char *)&my_creds, 0, sizeof(my_creds));
 my_creds.client = me;
 if((code=krb5_build_principal_ext(kcontext, &server,
	krb5_princ_realm(kcontext, me)->length,
	krb5_princ_realm(kcontext, me)->data,
	tgtname.length, tgtname.data,
	krb5_princ_realm(kcontext, me)->length,
	krb5_princ_realm(kcontext, me)->data,
	0))) {
#ifdef DEBUG
  com_err("krbauth", code, "while building server name");
#endif
  return 0; }

 my_creds.server = server;
 my_creds.times.starttime = 0;
 my_creds.times.endtime = now + KRB5_LIFETIME;
 if((code=krb5_get_in_tkt_with_password(kcontext, 0, 0,
	NULL, NULL, pass, 0, &my_creds, 0))) {
#ifdef DEBUG
  if(code==KRB5KRB_AP_ERR_BAD_INTEGRITY)
   com_err("krbauth", code, "incorrect password");
  else
   com_err("krbauth", code, "while getting initial credentials");
#endif
  return 0;
 }

 return 1;
}
