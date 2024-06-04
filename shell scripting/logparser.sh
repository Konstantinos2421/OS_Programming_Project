function mining_usernames {
	declare -a usernames
	usernames=($(awk -F - 'BEGIN { users_array[1] = "" } 
		{
			found=0
		    user=""
		    if($2 == " ") 
		    	{user = "-"}
		    else
		     	{user = $2}
		     
		    for(i in users_array){
		    	if(match(users_array[i], user)) {found=1}
		    } 

		    if(found==0){users_array[length(users_array)+1] = user} 
		}
		END {for(i in users_array) {print users_array[i]}}' $1))
	
	sorted_usernames=($(echo "${usernames[@]}" | tr ' ' '\n' | sort))
	IFS=' '
	usernames_string="${sorted_usernames[*]}"
	unset usernames
	unset sorted_usernames

	awk -F- -v usernames_string="$usernames_string" 'BEGIN { {split(usernames_string, usernames, " ")} {elements=0} {for (i in usernames) elements++} 
													for(i=1; i<=elements; i++) {counters[i]=0} }						      
							{
							    user=""
							    if($2 == " ") 
								user = "-"
							    else
							        user = $2
							         
							    gsub(/^[[:space:]]+/, "", user)
							    gsub(/[[:space:]]+$/, "", user)
							     	  
							    j = 0
							    for (i in usernames) {
    							    if (usernames[i] == user) {
        							    j = i
        							    break
    								}
							    }
							     
							    counters[j]++
						    }
						    END{ for(i=1; i<=elements; i++) {print counters[i] "\t" usernames[i]} }' $1
}

function count_browsers {
	awk 'BEGIN{{mozilla_count=0} {chrome_count=0} {safari_count=0} {edg_count=0}} 
	    {
	        if(match($0, "Mozilla")) {mozilla_count++} 
		 	else if(match($0, "Chrome")) {chrome_count++} 
		 	else if(match($0, "Safari")) {safari_count++} 				
		 	else if(match($0, "Edg")) {edg_count++}
	    } 
	    END {{print "Mozilla " mozilla_count} {print "Chrome " chrome_count} {print "Safari " safari_count} {print "Edg " edg_count}}' $1
}

case $* in
	*".log") sed 'p' $1
	         ;;
	*".log --usrid") mining_usernames $1
			 		 ;;
	*".log --usrid"*) awk -v user_id=$3 '{ if($0 ~ user_id) print $0 }' $1
			  		  ;;
	*".log -method"*) case $# in
						3) case $3 in
							GET) awk '/GET/ {print}' $1
					     	;;
							POST) awk '/POST/ {print}' $1
					      	;;
						*) echo "Wrong Method Name"
				    	esac
				    	;;
					*) echo "Wrong Method Name"
			    	esac
			    	;;
	*".log --servprot"*) case $# in
			     			3) case $3 in
			 	  				IPv4) awk '/127.0.0.1/ {print}' $1
			 	  	      		;;
			 	  				IPv6) awk '/::1/ {print}' $1
			 	  	      		;;
			 	  	  			*) echo "Wrong Network Protocol"
				      			esac
				      			;;
			     	  *) echo "Wrong Network Protocol"
	                  	 esac
	                     ;;
	*".log --browsers") count_browsers $1
			    		;;
	*".log --datum"*) case $# in
							3) case $3 in
								Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec) awk -v month=$3 '{if($0 ~ month) print $0}' $1
								;;
							*) echo "Wrong Date"
				   			   esac
				   			   ;;
				   *) echo "Wrong Date"
			  		  esac
			  		  ;;
	*) echo "Wrong File Argument"
esac
