#!/bin/bash

thumbdir=thumbs
thumbsize=150
thumbtype=png
thumbopts="-resize ${thumbsize}x${thumbsize}"
normdir=normal
normsize=450
normtype=png
normopts="-resize ${normsize}x${normsize}"
fulldir=full
formatdir=format
formats="pdf png jpg"
ncols=3

# Relative directories under basedir
styledir=/style

# Interface
title="Qweak Collaboration Figure Database"
sep="&bull;"
Sectionname="Category"
sectionname="category"
Sectionsname="Categories"
sectionsname="categories"
Itemname="Item"
itemname="item"
Itemsname="Items"
itemsname="items"

# Options:
# - include comments yes or no
includecomments=yes
# - enter comments yes or no
entercomments=no
# - server-side include yes or no
includes=no


# Style filenames (shtml contains SSI instructions)
header=header.shtml
titlebar=titlebar.html
sidebartop=sidebartop.html
sidebarmid=sidebarmid.shtml
sidebarbot=sidebarbot.html
footer=footer.html

# Name of index files
indexfile=index.html

# Filter of items to consider
filter="\(jpg\|png\|eps\|pdf\)"

# File with sidebar categories
sidebarfile=sidebar.html


# Source configuration file
source ${0/sh/conf}


# Function to do server-side includes on html generation
writehtml () {
	file=$1
	touch "$basedir/$file"
	if [ "$includes" == "yes" ] ; then
		echo "<!--#include virtual=\"$baseweb/$file\" -->"
	else
		while read line; do
			includefile=""
			if [[ "$line" =~ "^<!--#include.*" ]] ; then
				includefile=$(echo "${line/!/}" | sed -e "s|.*include.*=\"$baseweb\(.*\)\".*|\1|")
			fi
			if [ "$includefile" != "" ] ; then
				writehtml "$includefile"
			else
				echo "$line"
			fi
		done < "$basedir/$file"
	fi
}


# Function to determine relative path
relative () {
	relpath=${1/"$2"/.}
	relpath=$(dirname "$relpath")/$(basename "$relpath")
	relpath=${relpath/\/.\//\/}
	echo ${relpath/.\//}
}



process_dirs_in_dir () {

	local parent="$1"

	local nparentdirs=$(find "$origdir/$parent" -maxdepth 1 -type d | wc -l)
	let nparentdirs=$nparentdirs-1
	if [ "$ndirs" != 0 ] ; then
	{
		echo "<h3>Sub$sectionsname:</h3>"
		echo "<ul>"
	} >> "$indexfile".new
	fi

	# Main page sidebar header
	touch "$sidebarfile"
	echo "<li><a href=\"$baseweb/$indexfile\" id=\"new\">Home</a>" >> "$sidebarfile".new
	if [ "$parent" != "." ] ; then
		echo "<li><a href=\"$baseweb/${parent/\/*/}\" id=\"new\">${parent/\/*/}</a>" >> "$sidebarfile".new
	fi

	find "$origdir/$parent" -mindepth 1 -maxdepth 1 -type d -print0 | while read -d $'\0' dir ; do

		local section=$(relative "$dir" "$origdir")

		# Create working directory if necessary
		if [ ! -d "$section" ] ; then mkdir -p "$section" ; fi

		local nfiles=`find "$origdir/$section" -maxdepth 1 -type f -regex ".*\.$filter" | wc -l`
		local ndirs=`find "$origdir/$section" -maxdepth 1 -type d | wc -l`
		let ndirs=$ndirs-1

		# Categories
		echo "<li><a href=\"$baseweb/$section/$indexfile\">`basename "$section"` ($ndirs/$nfiles)</a>" >> "$sidebarfile".new

		# Line with number of entries
		echo "<li><a href=\"$baseweb/$section/$indexfile\">`basename "$section"`</a> ($ndirs $sectionsname, $nfiles $itemsname)" >> "$indexfile".new

		# Process this directory
		process_dir "$section"

	done

	mv "$sidebarfile".new "$sidebarfile"

	if [ "$nparentdirs" != 0 ] ; then
	{
		echo "</ul>"
	} >> "$indexfile".new
	fi
}


process_file () {

	local item=$1
	local prev=$2
	local next=$3

	local itemframe="$item.html"
	local prevframe="$prev.html"
	local nextframe="$next.html"

	# Do some processing on this item
	thumbitem="$item.$thumbtype"
	normitem="$item.$normtype"
	if [ ! -e "$thumbdir/$thumbitem" ] ; then
		# Autorotate jpg files
		if [ `hash exifautotran 2>/dev/null` ] ; then
			exifautotran "$fulldir/$item"
		fi

		# Convert in other sizes
		convert "$fulldir/$item" $thumbopts "$thumbdir/$thumbitem"
		convert "$fulldir/$item" $normopts "$normdir/$normitem"

		# Convert to other formats
		for format in $formats ; do
			convert "$fulldir/$item" "$formatdir/${item/.*/}.$format"
		done
	fi

	# Create html frame for this item
	touch "$itemframe".new
	{
		writehtml "$styledir/$header"
		echo "$title $sep $item"
		writehtml "$styledir/$titlebar"
		echo "$item"
		writehtml "$styledir/$sidebartop"
		writehtml "$section/$sidebarfile"
		writehtml "$styledir/$sidebarbot"
		echo "<p align=\"center\">"
	} >> "$itemframe".new


	# Navigation buttons
	if [ "$prev" != "0" ] ; then
		cat << END >> "$itemframe".new
<a href="$prevframe"><img border="0" src="$baseweb/$styledir/1leftarrow.png" title="Previous"></a>
END
	else
		cat << END >> "$itemframe".new
<img border="0" src="$baseweb/$styledir/1leftarrow2.png" title="Previous">
END
	fi

	cat << END >> "$itemframe".new
<a href="../$indexfile"><img border="0" src="$baseweb/$styledir/2uparrow.png" title="$Sectionname list"></a>
<a href="$indexfile"><img border="0" src="$baseweb/$styledir/1uparrow.png" title="This $sectionname"></a>
END

	if [ "$next" != "0" ] ; then
		cat << END >> "$itemframe".new
<a href="$nextframe"><img border="0" src="$baseweb/$styledir/1rightarrow.png" title="Next"></a>
END
	else
		cat << END >> "$itemframe".new
<img border="0" src="$baseweb/$styledir/1rightarrow2.png" title="Next">
END
	fi


	# Picture
	cat << END >> "$itemframe".new
</p>

<p align="center">
 <a href="$baseweb/$section/$fulldir/$item">
  <img src="$baseweb/$section/$normdir/$item.$normtype" width="450px">
 </a>
</p>
END


	# Other formats
	echo "<p align=\"center\">" >> "$itemframe".new
	echo "| <a href=\"$baseweb/$section/$fulldir/$item\">original</a>" >> "$itemframe".new
	for format in $formats ; do
		echo "| <a href=\"$baseweb/$section/$formatdir/${item/./_}.$format\">$format</a>" >> "$itemframe".new
	done
	echo " |</p>" >> "$itemframe".new	


	# Include comments if enabled
	if [ "$includecomments" == "yes" ] ; then
		commentsfile="${item}.comments.html"
		if [ ! -s "$basedir/$section/$commentsfile" ] ; then
			echo "<\!-- DO NOT REMOVE THIS LINE -->" > "$basedir/$section/$commentsfile"
		fi
		if [ -r "$origdir/$section/$commentsfile" ] ; then
			echo "    [comment added from $origdir/$section/$commentsfile]"
			cp -f "$origdir/$section/$commentsfile" "$basedir/$section/$commentsfile"
			writehtml "$section/$commentsfile" >> "$itemframe".new
		fi
	fi


	# Enter comments if enabled
	if [ "$entercomments" == "yes" ] ; then
		echo "<hr />" >> "$itemframe".new

		cat << END >> "$itemframe".new
<form action="/cgi-bin/guestbook.pl" method="POST" target="_self">
  <input type="hidden" name="successURL" value="http://qweak.jlab.org/$section/$itemframe">
  <input type="hidden" name="ccme" value="yes">
  <input type="hidden" name="filename" value="$section/$commentsfile">
  <input type="hidden" name="ccmeSubject" value="Comment on $itemname $item in $sectionname $section">
  <table align="center">
    <tr>
      <td colspan="2">
        <textarea name="comments" rows="2" cols="60" wrap="soft" onfocus="if(this.value=='Comments on this item.')this.value=''">Comments on this item.</textarea>
      </td>
    </tr>
    <tr>
      <td>Your name:</td>
      <td align="right"><input type="text" size="30" name="realname"></td>
    </tr>
    <tr>
      <td><input type="submit" value="Submit"></td>
      <td align="right"><input type="reset"></td>
    </tr>
  </table>
</form>

END
	fi


	# Footer
	writehtml $styledir/$footer >> "$itemframe".new

	# Move into place
	mv "$itemframe".new "$itemframe"
}



process_files_in_dir () {

	local section=$1

	local nfiles=$(find "$origdir/$section" -maxdepth 1 -type f | wc -l)
	if [ "$nfiles" == 0 ] ; then return ; fi

	# Descend in this section
	if [ ! -d "$thumbdir" ] ; then mkdir -p "$thumbdir" ; fi
	if [ ! -d "$normdir" ] ; then mkdir -p "$normdir" ; fi
	if [ ! -d "$fulldir" ] ; then mkdir -p "$fulldir" ; fi
	if [ ! -d "$formatdir" ] ; then mkdir -p "$formatdir" ; fi

	{
		echo "<h3>$section:</h3>"
		echo "<p align=\"center\">"
		echo "<a href=\"../$indexfile\"><img border=\"0\" src=\"$baseweb/$styledir/1uparrow.png\" title=\"$Sectionname list\"></a>"
		echo "</p>"
		echo "<p align=\"center\">"
		echo "<table border=\"0\" cellpadding=\"7\" cellspacing=\"0\" width=\"100%\">"
	} >> "$indexfile".new

	local counter=0
	local horiz=1
	local prev="0"
	local next="0"
	local total=`find "$origdir/$section" -maxdepth 1 -type f -regex ".*\.$filter" | wc -l`
	find "$origdir/$section" -maxdepth 1 -type f -regex ".*\.$filter" -print0 | while read -d $'\0' file ; do

		let nprev=$counter
		let counter=$counter+1
		let nnext=$counter+1
		prev=`find "$origdir/$section" -maxdepth 1 -type f -regex ".*\.$filter" | head -n $nprev | tail -n 1`
		next=`find "$origdir/$section" -maxdepth 1 -type f -regex ".*\.$filter" | head -n $nnext | tail -n 1`
		if [ ! -z "$prev" ] ; then prev=`basename "$prev"` ; else prev="0" ; fi
		if [ ! -z "$next" ] ; then next=`basename "$next"` ; else next="0" ; fi
		if [ "$counter" == "$total" ] ; then next="0" ; fi

		if [ $horiz = 1 ] ; then
			echo "<tr align=\"center\" valign=\"middle\">" >> "$indexfile".new
			echo "<td>" >> "$indexfile".new
		else
			echo "<td>" >> "$indexfile".new
		fi

		item=`basename "$file"`
		echo "   $Itemname: $item ($prev ... X ... $next)"

		# Copy/link original
		if [ -L "$fulldir/$item" ] ; then rm "$fulldir/$item" ; fi
		cp -f "$file" "$fulldir/$item"

		process_file "$item" "$prev" "$next"

		local itemframe="$item.html"
		{
			echo "    <a href=\"$itemframe\">"
			echo "      <img src=\"$thumbdir/$item.$thumbtype\" border=\"0\">"
			echo "    </a><a href=\"$item\">&nbsp;</a>"
		} >> "$indexfile".new

		if [ $horiz = $ncols ] ; then
			echo "  </td>" >> "$indexfile".new
			echo "</tr>" >> "$indexfile".new
			horiz=1
		else
			echo "  </td>" >> "$indexfile".new
			let horiz=$horiz+1
		fi

	done

	{
		echo "</table>"
		writehtml $styledir/$footer
	} >> "$indexfile".new
}



process_dir () {

	local section=$1

	# Go to equivalent directory in basedir
	echo $Sectionname: $section
	if [ ! -d "$basedir/$section" ] ; then mkdir -p "$basedir/$section" ; fi
	pushd "$basedir/$section" > /dev/null

	# Link to index.html if different
	touch "$indexfile"
	if [ "$indexfile" != "index.html" ] ; then
		ln -sf "$indexfile" index.html
	fi


	# Headers and page organization
	{
		writehtml $styledir/$header
		if [ "$section" == "." ] ; then
			echo "$title"
		else
			echo "$title $sep $sectionname $section"
		fi
		writehtml "$styledir/$titlebar"
		echo "$Sectionname $section"
		writehtml "$styledir/$sidebartop"
		writehtml "$section/$sidebarfile"
		writehtml "$styledir/$sidebarbot"
	} >> "$indexfile".new


	# Process the directories in this directory
	process_dirs_in_dir "$section"


	# Process the files in this directory
	process_files_in_dir "$section"


	# Move the index file to active
	mv "$indexfile".new "$indexfile"


	# Pop back silently
	popd > /dev/null
}


##### START #####

# Create the base directory and copy style
mkdir -p "$basedir/style"
cp -u `dirname $0`/style/* "$basedir/style"

# Start processing at top level
process_dir "."

echo "Note: the sidebars will only be correct after a second pass."

chmod -R g+rw ${basedir}
chmod -R g-s  ${basedir}
