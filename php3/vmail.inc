<?
// Copyright (C) 2000 Mike Bell <mike@mikebell.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

function vm_daemon_raw($arg)
{
        $vmailfile = "/tmp/.vmailmgrd";
	if (file_exists("/etc/vmailmgr/socket-file"))
		$socketfile = (file ("/etc/vmailmgr/socket-file"));
		$socketfile = trim($socketfile[0]);
		if ($socketfile != "")
			$vmailfile = $socketfile;
		

        $vmailsock = fsockopen ($vmailfile, 0, $errno, $errstr, 4);
	if (!$vmailsock) die("Failed to open socket file, is daemon running?");

/*      Parse $arg, which should be an array of arguments to pass to the
        daemon, into a glob consisting of each argument proceeded by a
        two-byte representation of its length. */

        for ($x=0; $x < sizeof($arg); $x++ )
        {
                $commandlength = strlen($arg[$x]);
                $high=(($commandlength & (0xFF << 8)) >> 8);
                $low=($commandlength & 0xFF);
                $command .= sprintf("%c%c%s", $high, $low, $arg[$x]);
        };


/*      Create the header, which consists of another two byte length
        representation, the number of arguments being passed, and the
        command string created above. */

        $args=$x-1;
        $commandlength=strlen($command);
        $high=(($commandlength & (255 << 8)) >> 8);
        $low=($commandlength & 255);
        $commandstr = sprintf("\002%c%c%c", $high, $low+1, $args).$command;

/*      Pass it all to the daemon */

        fputs($vmailsock, $commandstr);

/*      Get the response        */

        for ($x=0; ; $x++)
        {
                $out[$x] = fgetc($vmailsock);
		if (feof($vmailsock)) break;
        }

/*	Close the socket	*/
	fclose($vmailsock);

	unset($out[$x]);

/* 	If output is a successful listdomain, it comes in a special format to
	keep clients from returning it as a success message. Pass it back raw
	to facilitate this */

if ($arg[0]=="listdomain")
	if (ord($out[0]) == 0) return $out;


	$value = (int) $out[0];
	$out[0]=""; $out[1]=""; $out[2]="";
	$output=join("", $out);
	$return[]=$value;
	$return[]=$output;
return $return;

}

/* Returns the contents of one field in $out beginning at $pos. */

function listdomain_parse_field(&$out, &$pos){
if (ord($out[$pos])==0) {$pos++;return 0;}
	for (; ord($out[$pos]) != 0; $pos++){
		$field.=$out[$pos];
	}
$pos++;
return $field;
}

/* Calls listdomain_parse_field repeatedly to parse a single line of the
   listdomain output */

function listdomain_parse_line(&$out, $pos){
	$username=listdomain_parse_field(&$out, &$pos);
	$password=listdomain_parse_field(&$out, &$pos);
	$mbox=listdomain_parse_field(&$out, &$pos);
	if ($mbox=="0") {$mbox="";}
	for ($x=0; ; $x++){
		$temp=listdomain_parse_field(&$out, &$pos);
		if (!$temp) break;
		$alias[$x]=$temp;
	}
return array($username, $password, $mbox, $alias);
}

/* Returns an array of lines, which are in turn arrays of 
   (username, password, mbox, alias) */

function listdomain_parse_output(&$out){
	$pos=4;
	$length=1;
	for ($x=0; ; $x++){
		$length=(ord($out[$pos]) << 8) + ord($out[++$pos]);
		if ($length == 0) break;
		list($username, $password, $mbox, $alias)=listdomain_parse_line($out, ++$pos, $length);
		$pos=$pos+$length+1;
		$output[$x]=array($username, $password, $mbox, $alias);
	}
	return $output;
}

/* listdomain, takes domain name and password, returns listdomain output
   parsed as stated in listdomain_parse_output */

function listdomain($domain, $password){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
	$command=array("listdomain",$domain, $password);
	$temp=vm_daemon_raw(array("listdomain", $domain, $password));
	if (ord($temp[0]) != 0) return $temp;
return listdomain_parse_output($temp);
}

/* vadduser, takes domain name, password, username, userpassword, and an
   array of forwarding desinations, returns an array consisting of an
   integer exit code and message. */

function vadduser($domain, $password, $username, $userpass, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
if ($userpass=="") return array(1, "No user password supplied");
if ($forwards[0]=="") return array(1, "Empty forwarding address");
	$command=array("adduser2", $domain, $username, $password,
	               $userpass, $username);
	for ($x=0; $x < sizeof($forwards); $x++)
		if ($forwards[$x]!="")
			$command[]=$forwards[$x];
	$temp=vm_daemon_raw($command);
return $temp;
}

/* vaddalias, takes domain name, password, username, userpassword, and an
   array of forwarding desinations, returns an array consisting of an
   integer exit code and message. If the user's password is left empty an
   alias with no password will be created. */

function vaddalias($domain, $password, $username, $userpass, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
if ($forwards[0]=="") return array(1, "Empty forwarding destination");
	$command=array("adduser2", $domain, $username, $password,
	               $userpass, "");
	for ($x=0; $x < sizeof($forwards); $x++)
		if ($forwards[$x]!="")
			$command[]=$forwards[$x];
	$temp=vm_daemon_raw($command);
return $temp;
}

/* vdeluser, takes domain name, password, and username, returns an array
   consisting of an integer exit code and message. */

function vdeluser($domain, $password, $username){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
	$command=array("deluser", $domain, $username, $password);
	$temp=vm_daemon_raw($command);
return $temp;
}

/* vchpass, takes domain name, password, username and a new password,
   returns an array consisting of an integer exit code and message. Scripts
   allowing users to change their own passwords should check the password
   was entered correctly by having the user enter it twice and checking
   these are equal*/

function vchpass($domain, $password, $username, $newpass){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
if ($newpass=="") return array(1, "Empty new password");
	$command=array("chattr", $domain, $username, $password, "1", $newpass);
	$temp=vm_daemon_raw($command);
return $temp;
}

/* vchforward, takes domain name, password, username and an array of
   forwarding addresses, returns an array consisting of an integer exit code
   and message. Due to a bug in PHP when dealing with array values
   containing NULs, vchforward will not handle multiple destinations unless
   one is using the patched vmailmgrd present in 0.96.2 and later */

function vchforward($domain, $password, $username, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
	$command=array("chattr", $domain, $username, $password, "2");
	for ($x=0; $x< sizeof($forwards); $x++)
		if ($forwards[$x] != "") $command[]=$forwards[$x];
	$temp=vm_daemon_raw($command);
return $temp;
}

?>
