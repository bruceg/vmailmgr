<?
// Copyright (C) 2000 Mike Bell <mike@mikebell.org>
//
// Examples available at http://mailtest.mikebell.org/
// Latest version will be included in each vmailmgr release
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

	(int) $value = (int) fgetc ($vmailsock);
	$length = (ord(fgetc($vmailsock)) << 8) + ord(fgetc($vmailsock));

	if ($length == 0) {
		while (!feof($vmailsock))
			$out=fread($vmailsock, 65535);
		fclose($vmailsock);
		return $out;
	}

	$message = fgets ($vmailsock, $length);

/*	Close the socket	*/
	fclose($vmailsock);

return array($value, $message);

}

/* Parses the lines from listdomain into fields. All fields after aliases
   are ignored, but this is easy to fix if anybody cared about them */

function listdomain_parse_line($line){
	$user=explode(chr(0), $line);
	$username=$user[0];
	if ($user[1]!=(chr(1)."*")) $password="Set";
	$mailbox=$user[2];
	$x=3;
	while($user[$x]!="")
		$aliases[]=$user[$x++];
	$x++;
	$PersonalInfo=$user[$x++];
	$HardQuota=$user[$x++];
	$SoftQuota=$user[$x++];
	$SizeLimit=$user[$x++];
	$CountLimit=$user[$x++];
	$CreationTime=$user[$x++];
	$ExpiryTime=$user[$x++];
	
	return array($username, $password, $mailbox, $aliases,
$PersonalInfo, $HardQuota, $SoftQuota, $SizeLimit, $CountLimit,
$CreationTime, $ExpiryTime);
}

/* Does the ugly stuff for listdomain, and calls listdomain_parse_line once
   for each user */

function listdomain_parse($output){
	$cur=1;
	do {
		$linelength=(ord(substr($output, $cur++, 1)) << 8 ) + ord(substr($output, $cur++, 1));
		if ($linelength == 0) break;
		$array[] = listdomain_parse_line(substr($output, $cur, $linelength));
		$cur+=$linelength+1;
	} while (1);
	return $array;
}

/* listdomain, takes domain name and password, returns listdomain output
   parsed as stated in listdomain_parse */

function listdomain($domain, $password){
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
	$command=array("listdomain",$domain, $password);
	$temp=vm_daemon_raw(array("listdomain", $domain, $password));
	if (is_array($temp)) return $temp;
return listdomain_parse($temp);
}

/* vadduser, takes domain name, password, username, userpassword, and an
   array of forwarding desinations, returns an array consisting of an
   integer exit code and message. */

function vadduser($domain, $password, $username, $userpass, $forwards){
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
if ($username=="") return array(0, "Empty username");
if ($userpass=="") return array(0, "No user password supplied");
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
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
if ($username=="") return array(0, "Empty username");
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
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
if ($username=="") return array(0, "Empty username");
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
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
if ($username=="") return array(0, "Empty username");
if ($newpass=="") return array(0, "Empty new password");
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
if ($domain=="") return array(0, "Empty domain");
if ($password=="") return array(0, "Empty domain password");
if ($username=="") return array(0, "Empty username");
	$command=array("chattr", $domain, $username, $password, "2");
	for ($x=0; $x< sizeof($forwards); $x++)
		if ($forwards[$x] != "") $command[]=$forwards[$x];
	$temp=vm_daemon_raw($command);
return $temp;
}

?>
