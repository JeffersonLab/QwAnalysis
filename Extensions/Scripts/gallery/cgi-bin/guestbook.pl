#!/usr/bin/perl
################################################################################
#
# GUESTBOOK
#
################################################################################
# Authors:  
#      Brian G. Kuhn <bgkuhn@engin.umich.edu>
#      David Winkel <dwinkel@umich.edu>
#      Kevin McGowan <clunis@umich.edu>
#      
# History:
#      9/01/97 clunis: various mods for security and customization
#      11/20/96 dwinkel: heavy mods for customization and security
#      4/4/96 bgkuhn: Initial release
#
################################################################################
## Copyright (C) 1996 by the Regents of the University of Michigan.
##
## User agrees to reproduce said copyright notice on all copies of the software
## made by the recipient.
##
## All Rights Reserved. Permission is hereby granted for the recipient to make
## copies and use this software for its own internal purposes only. Recipient of
## this software may not re-distribute this software outside of their own
## institution. Permission to market this software commercially, to include this
## product as part of a commercial product, or to make a derivative work for
## commercial purposes, is explicitly prohibited.  All other uses are also
## prohibited unless authorized in writing by the Regents of the University of
## Michigan.
##
## This software is offered without warranty. The Regents of the University of
## Michigan disclaim all warranties, express or implied, including but not
## limited to the implied warranties of merchantability and fitness for any
## particular purpose. In no event shall the Regents of the University of
## Michigan be liable for loss or damage of any kind, including but not limited
## to incidental, indirect, consequential, or special damages.
################################################################################
 
################################################################################
# Initialization of Global variables

# URL where cgi script resides
$CGI_URL      = "http://qweak.jlab.org/cgi-bin/guestbook.pl";
$HELP_URL     = "http://qweak.jlab.org/index.html";
$SERVER_NAME  = "qweak.jlab.org";
$sendmail     = '/usr/lib/sendmail -t -oi';
chop($hostname=`hostname`) if  (-e "/bin/hostname");
chop($hostname=`uname -n`) if !(-e "/bin/hostname");


# Version number and date of last modification
$VERSION      = 1.2;
$LAST_UPDATE  = "9/01/97"; 

# HTML code for webmaster information
$WEBMASTER    =  "<em i><a href=\"http://qweak.jlab.org/index.html\">\n" .
                 "wdconinc\@jlab.org</em></a>\n";

# HTML code for version information
$VERSION_MESG =  "\n<hr><i>GuestBook v$VERSION</i><br>\n" .
                 "Report problems with this gateway to\n" . $WEBMASTER . 
                 "Online <a href=\"$CGI_URL/HELP\">help</a> \nis available.\n";

# Typical File system location (minus user home directory) where data is kept
$CGI_DIR = "/private/cgi-data";

# Date -- in pretty format
chop($DATE = `/bin/date +"%A, %B %d, %Y -- %T (%Z)"`);


# Other globals include 
# %FORM           -- for passing posted information
# $USER           -- user name for guestbook
# $GUESTBOOK_URL  -- URL of the guestbook for $USER
# $GUESTBOOK_DIR  -- File system directory for the html file
# $GUESTBOOK_HTML -- Html file for guestbook
################################################################################


######################################################################
# MAIN
######################################################################


# Determine if data was posted
if ($ENV{'REQUEST_METHOD'} eq 'GET')  { $buffer = $ENV{'QUERY_STRING'}; }
if ($ENV{'REQUEST_METHOD'} eq 'POST') { read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'}); }

# List normal valid fields
@normalfields = ('url', 'realname', 'email', 'city', 'state', 'country',
                 'comments', 'successURL', 'allowHTML', 'ccme', 'mailSubject',
		 'filename');

# Get USER and set URL of USER's GuestBook
$USER = $1 if $ENV{'PATH_INFO'} =~ m@/(.*)@;
$GUESTBOOK_URL  = "http://$SERVER_NAME/~$USER/cgi-data/";

&Help if (!$USER || $USER eq 'HELP');        # Display Help if no USER or USER is HELP 
$FORM{'filename'} = "guestbook.html";

&Form unless $buffer;                        # No data posted, so display default form

# Data was posted so parse the input
$FORM{'ccme'} = "no";
$FORM{'mailSubject'} = "Guestbook notification";
@pairs = split(/&/, $buffer);	             # Break down buffer into key=value
foreach $pair (@pairs) {
    ($name, $value) = split(/=/, $pair);     # Break out key and value
    $value =~ tr/+/ /;                       # Get rid of pluses and hex codes
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $value =~ s/<!--(.|\n)*-->//g;           # Get rid of comment identifiers
    $value =~ s/&/&amp/g;
    if (grep(/^$name$/i,@normalfields)) {
        $FORM{$name} = $value;               # Set the key and value hash
    } else {
        $FORMPLUS{$name} = $value;
        push (@addfields, $name);
    }
}

# A little security check...
$FORM{'filename'} =~ s/\///g;


# Grab home directory out of password file, determine DIR and HTML for GuestBook
$GUESTBOOK_DIR = (getpwnam($USER))[7] || &Error("Unable to get user information...");
$GUESTBOOK_DIR .= $CGI_DIR;
$GUESTBOOK_HTML = $GUESTBOOK_DIR . "/" . $FORM{'filename'};
$GUESTBOOK_URL .= $FORM{'filename'};

&SanityChecking;		             # Do some sanity checking

# Get current GuestBook OR complain that file doesn't exist.
if (-z $GUESTBOOK_HTML) {
	@LINES = &InitBook;
} elsif (-e $GUESTBOOK_HTML) {
	open(F, $GUESTBOOK_HTML);
	@LINES = <F>;
	close(F);	
} else {
	&Error("No Guestbook exists for writing at $GUESTBOOK_URL.\n If you are the owner of this guestbook, please refer to: <A HREF=\"$HELP_URL\">$HELP_URL</A><P>\n");
}

# Write new contents of GuestBook
open(F, ">$GUESTBOOK_HTML");
foreach (@LINES) {
    print F "$_";
    if (/<!-- DO NOT REMOVE THIS LINE -->/) {
	print F "<hr>$DATE<br>\n";
	
	# Print name, url, email address of commentor
	print F "Name:  ";
	($FORM{'url'} && $FORM{'url'} =~ /^http/) 
	    ? print F "<a href=\"$FORM{'url'}\">$FORM{'realname'}</a>" 
	    : print F "$FORM{'realname'}";
	print F " -- Email:  <a href=\"mailto:$FORM{'email'}\">$FORM{'email'}</a>" if $FORM{'email'};
	print F "<br>\n";

	# Print city, state, country
        if ($FORM{'city'} || $FORM{'state'} || $FORM{'country'}) {
            if ($FORM{'allowHTML'} =~ /^[yY]/) {
    	        print F "Location: $FORM{'city'}, $FORM{'state'}  $FORM{'country'}<br>\n";
            } else {
                $FORM{'city'} =~ s/</&lt/g;
                $FORM{'city'} =~ s/>/&gt/g;
                $FORM{'state'} =~ s/</&lt/g;
                $FORM{'state'} =~ s/>/&gt/g;
                $FORM{'country'} =~ s/</&lt/g;
                $FORM{'country'} =~ s/>/&gt/g;
  	        print F "Location: $FORM{'city'}, $FORM{'state'}  $FORM{'country'}<br>\n";
            }
        }

        # Print Comments
        if ($FORM{'allowHTML'} =~ /^[yY]/) {
	    print F "<blockquote> $FORM{'comments'}</blockquote>\n";
        } else {
            $FORM{'comments'} =~ s/</&lt/g;
            $FORM{'comments'} =~ s/>/&gt/g;
            $FORM{'comments'} =~ s/\n/<BR>\n/g;
	    print F "<li><b>$FORM{'comments'}</b><br>\n";
        }
        foreach $field (@addfields) {
            if ($FORM{'allowHTML'} =~ /^[yY]/) {
                print F "<li>$field = $FORMPLUS{$field}<BR>\n";
            } else {
                $FORMPLUS{'$field'} =~ s/</&lt/g;
                $FORMPLUS{'$field'} =~ s/>/&gt/g;
                $FORMPLUS{'$field'} =~ s/\n/<BR>\n/g;
                print F "<li>$field = $FORMPLUS{$field}<BR>\n";
            }
        }
    }
}    
close F;
if ($FORM{'ccme'} =~ /^[yY]/) {
    open(M,"|$sendmail") || die "sendmail: $!";

    print M "From: nobody\@umich.edu\n";
    print M "To: $USER\@umich.edu\n";
    print M "Subject: $FORM{'mailSubject'}\n";
    
    print M "\n";
    print M "The following entry was added to your guestbook at:\n";
    print M "  $GUESTBOOK_URL\n\n";

    # Print name, url, email address of commentor
    print M " --  Name:  $FORM{'realname'}\n";
    print M " -- Email:  $FORM{'email'}\n" if $FORM{'email'};
    print M " --   URL:  $FORM{'url'}\n" if $FORM{'url'};
    print M " Note: $FORM{'url'} could not be used as a hypertext link and was not added to your guestbook.\n" if ( $FORM{'url'} !~ m/^http:/i );
    print M "\n";

    # Print city, state, country
    print M "Location: $FORM{'city'}, $FORM{'state'}  $FORM{'country'}\n"
	if ($FORM{'city'} || $FORM{'state'} || $FORM{'country'});

    # Print Comments
    print M "$FORM{'comments'}\n";
    foreach $field (@addfields) {
        print M "$field = $FORMPLUS{$field}\n";
    }
    
    print M "\n---------------------------------------------------------------------------\n\n";
    
    print M "NOTE: This message was gatewayed through $hostname on an\n";
    print M "HTTP request from $ENV{'REMOTE_HOST'}.\n";
    print M "No attempt has been made to verify the sender's identity.\n\n";
    close M;
}

if ($FORM{'successURL'}) {
    print "Location:  $FORM{'successURL'}\n\n";
} else {
    &Success;
}
exit;
 

######################################################################
# SanityChecking
######################################################################
sub SanityChecking {
    # Make sure comments were entered
    &Error("Comments field appeared to be blank, entry not added to GuestBook...")
	unless $FORM{'comments'};

    # Make sure name was entered
    &Error("Name field appeared to be blank, entry not added to GuestBook...")
	unless $FORM{'realname'};

    # Make sure the container directory exists
    &Error("$USER is not configured for the GuestBook services...") 
	if (!-d $GUESTBOOK_DIR);

    # Make sure we can write to the file
    &Error("$USER is not allowing entries into a GuestBook...")
        if (!open(F, ">$GUESTBOOK_DIR/write-check"));
    unlink("$GUESTBOOK_DIR/write-check");
}        

######################################################################
# Error
######################################################################
sub Error {
    # Print initial content type 
    print "Content-type: text/html\n\n\r";

    print "<html><head><title>GuestBook Error</title></head><body>";
    print "<h1>Error!</h1>@_<p>$VERSION_MESG</body></html>";
    exit 1;
}

######################################################################
# Success
######################################################################
sub Success {
    # Print initial content type 
    print "Content-type: text/html\n\n\r";

    print "<html><head><title>GuestBook Entry</title></head><body>";
    print "<h2>Entry Made!</h2>";
    print "Your message has been successfully entered in the guestbook ";
    print "of <b>$USER</b>.<br>";
    print "Click <a href=\"$GUESTBOOK_URL\">here</a> to see the GuestBook.<p>";
    print "$VERSION_MESG </body></html>\n";  
}

######################################################################
# InitBook
######################################################################
sub InitBook {
    ("<html>                                                \n",
     "<head><title>GuestBook of $USER</title></head>        \n",
     "<body>                                                \n",  
     "<h1>GuestBook</h1>                                    \n\n",
     "Thank you for visiting!  You can click                \n",
     "<a href=\"$CGI_URL/$USER\">here</a>                   \n",
     "to add to this GuestBook.<p>                          \n\n",
     "<!-- DO NOT REMOVE THIS LINE -->                      \n\n",
     "<p>$VERSION_MESG</body></html>");
}
	    
######################################################################
# Form
######################################################################
sub Form {
    # Print initial content type 
    print "Content-type: text/html\n\n\r";

    print "<html>\n";
    print "<head><title>GuestBook entry for $USER</title></head>\n\n";
    print "<h2>Thank you for visiting!</h2>\n\n";
    print "Fill in the form below to add to the guestbook of <b>$USER</b>.\n";
    print "Your name and comments are required to add your entry successfully.\n\n";
    print "<hr><form method=POST action=\"$CGI_URL/$USER\">\n";
    print "Your Name:  <input type=text name=\"realname\"  size=30>                     <br>\n";
    print "E-Mail:     <input type=text name=\"email\"     size=40>                     <br>\n";
    print "URL:        <input type=text name=\"url\"       size=60>                     <br>\n";
    print "City:       <input type=text name=\"city\"      size=15>                         \n";
    print "State:      <input type=text name=\"state\"     size=15>                         \n";
    print "Country:    <input type=text name=\"country\"   size=15>                     <p> \n";
    print "Comments:   <br>   <textarea name=\"comments\"  cols=60  rows=4></textarea>  <p> \n";
    print "<input type=submit value=\"Submit\"> <input type=reset>                          \n";
    print "</form>\n\n";
    print "<hr>Return to the \n<a href=\"$GUESTBOOK_URL$FORM{'filename'}\">GuestBook</a> \nfor <b>$USER</b>. \n";
    print "<p>$VERSION_MESG</html>\n";
    exit;
}

######################################################################
# Help
######################################################################
sub Help {
    # Print redirect to documentation
    print "Location: $HELP_URL\n\n\r";
    exit;
}

