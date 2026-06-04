target("mic-player")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
	add_links("notcurses", "notcurses-core")	
