osascript -e 'tell app "Address Book" to get the name of every person' | perl -pe 's/, /\n/g' | sort | uniq -d
