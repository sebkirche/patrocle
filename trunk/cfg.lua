-- -*- coding: utf-8 -*-

debug = 2
maintainer_addr = "sebastien.kirche@free.fr"
maintainer_name = "seki"
--notefile = path_to_notefile
helpfile = "bot/help.bot"
--idletimeout = 42
--waittimeout = 42
--maxuploadsize = 1024

FileList = { 
   name = "default",
   userlist = "user.list",
   protlist = "prot.list",
   shitlist = "shit.list",
   rellist	= "rel.list"
}

--Bots is a table of bots, each bot is a table of parameters
Bots = {
   {
	  id = "patrocle",				--internal id
	  nick = "patrocle",			--bot nickname
      name = "/msg Patrocle help",	--bot name for other people
      login = "patrocle",			--login name for the server
      channels = {
		--{name = "#emacsfr", mode = "", topic = "", encoding = "UTF-8"},
		{name = "#seki", mode = "",topic = "",	encoding = "iso-8859-1"},
		 {name = "#patrocle", mode = "",topic = "Bienvenue chez Patrocle, le bot parlant",	encoding = "utf-8"}
      },
      servers = {"irc.freenode.org"},
      stims = "Citations.stim",
      reps = "Citations.reps",
      botlist = "bot.list",
      index =  "INDEX",
      --download = "/dev/null",
      --upload = "/dev/null",
      fileset = FileList
   },
--[[
   {
	  id = "patrocle_nerim",				--internal id
	  nick = "patrocle",			--bot nickname
      name = "/msg Patrocle help",	--bot name for other people
      login = "patrocle",			--login name for the server
      channels = {
		--{name = "#emacsfr", mode = "", topic = "", encoding = "UTF-8"},
		--{name = "#seki", mode = "",topic = "",	encoding = "iso-8859-1"},
		 {name = "#patrocle", mode = "",topic = "Bienvenue chez Patrocle, le bot parlant",	encoding = "utf-8"}
      },
      servers = {"ircnet.nerim.fr"},
      stims = "Citations.stim",
      reps = "Citations.reps",
      botlist = "bot.list",
      index =  "INDEX",
      --download = "/dev/null",
      --upload = "/dev/null",
      fileset = FileList
   }
]]   
}
