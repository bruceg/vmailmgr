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

	$value = ord( fgetc ($vmailsock));
	$length = (ord(fgetc($vmailsock)) << 8) + ord(fgetc($vmailsock));

	if ($length == 0) {
		while (!feof($vmailsock))
			$out=fread($vmailsock, 65535);
		fclose($vmailsock);
		return $out;
	}

	$message = fread ($vmailsock, $length);

/*	Close the socket	*/
	fclose($vmailsock);

return array($value, $message);

}

/* Parses the lines from listdomain into fields. All fields after aliases
   are ignored, but this is easy to fix if anybody cared about them */

function listdomain_parse_line($line){
	//Grab the user's name
	$position = strpos($line, chr(0));
	$username=substr($line, 0, $position);
	//Grab the protocol version
	$ver=ord(substr($line, $position+1, $position+2));
	if ($ver != 2) die("Protocol version is $ver, not the 2 as this script expects.");
	//Thow away everything already done
	$line=substr($line, $position+2);
	//Grab the flags
	while (substr($line, 0, 1) != chr(0)){
		$flags[ord(substr($line, 0, 1))]=ord(substr($line, 1, 1));
		$line=substr($line, 2);
	};
	//Explode the remainder into an array based on the NULs used to end the fields
	$user=explode(chr(0), $line);
	$x=1;
	if ($user[$x++]!=("*")) $password="Set";
	$mailbox=$user[$x++];
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
$CreationTime, $ExpiryTime, $flags);
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
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
	$command=array("listdomain",$domain, $password);
	$temp=vm_daemon_raw(array("listdomain", $domain, $password));
	if (is_array($temp)) return $temp;
return listdomain_parse($temp);
}

/* vadduser, takes domain name, password, username, userpassword, and an
   array of forwarding desinations, returns an array consisting of an
   integer exit code and message. */

function vadduser($domain, $password, $username, $userpass, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
if ($userpass=="") return array(1, "No user password supplied");
	$command=array("adduser2", $domain, $username, $password,
	               $userpass, $username);
	for ($x=0; $x < sizeof($forwards); $x++)
		if ($forwards[$x]!="") $command[]=$forwards[$x];
return vm_daemon_raw($command);
}

/* vaddalias, takes domain name, password, username, userpassword, and an
   array of forwarding desinations, returns an array consisting of an
   integer exit code and message. If the user's password is left empty an
   alias with no password will be created. */

function vaddalias($domain, $password, $username, $userpass, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
	$command=array("adduser2", $domain, $username, $password,
	               $userpass, "");
	for ($x=0; $x < sizeof($forwards); $x++)
		if ($forwards[$x]!="")
			$command[]=$forwards[$x];
return vm_daemon_raw($command);
}

/* vdeluser, takes domain name, password, and username, returns an array
   consisting of an integer exit code and message. */

function vdeluser($domain, $password, $username){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
	$command=array("deluser", $domain, $username, $password);
return vm_daemon_raw($command);
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
return vm_daemon_raw($command);
}

/* vchforward, takes domain name, password, username and an array of
   forwarding addresses, returns an array consisting of an integer exit code
   and message. */ 

function vchforward($domain, $password, $username, $forwards){
if ($domain=="") return array(1, "Empty domain");
if ($password=="") return array(1, "Empty domain password");
if ($username=="") return array(1, "Empty username");
	$command=array("chattr", $domain, $username, $password, "2");
	for ($x=0; $x< sizeof($forwards); $x++)
		if ($forwards[$x] != "") $command[]=$forwards[$x];
	if (sizeof($forwards) == 0) $command[] = "";
return vm_daemon_raw($command);
}

/* v*/

function vchattr($domain, $password, $username, $attr, $value){
	$ATTR = array(
		"PASS" => "1",
		"DEST" => "2",
		"HARDQUOTA" => "3",
		"SOFTQUOTA" => "4",
		"MSGSIZE" => "5",
		"MSGCOUNT" => "6",
		"EXPIRY" => "7",
		"MAILBOX_ENABLED" => "8",
		"PERSONAL" => "9"
	);
	$command = array ("chattr", $domain, $username, $password, $ATTR[$attr], $value);
return vm_daemon_raw($command);
}

function vwriteautoresponse($domain, $password, $username, $message){
	$command = array("autoresponse", $domain, $username, $password, "write", $message);
return vm_daemon_raw($command);
}

function vreadautoresponse($domain, $password, $username){
	$command = array("autoresponse", $domain, $username, $password, "read");
return vm_daemon_raw($command);
}

function vdisableautoresponse($domain, $password, $username){
	$command = array("autoresponse", $domain, $username, $password, "disable");
return vm_daemon_raw($command);
}

function venableautoresponse($domain, $password, $username){
        $command = array("autoresponse", $domain, $username, $password, "enable");
return vm_daemon_raw($command);
}


?>
