--traitement des messages  -*- coding: utf-8 -*-


require("misc")

--déclencheurs
Flags = {}

Triggers = {
   {{"sboub"}, 
    "SBOUB"},

   {{"ils font "}, 
    "ILS_FONT"},

   {{function (from,to,msg) return chaine_est_dans(msg, BotNick) end,
	 {"tous", "^tous les "},
	 "a ts", "les gars", "les men", "les mecs", "tout-le-monde", "tout le monde", "tout l'monde", "tt le monde", "tt l'monde", "tt l monde", "le bot", "les bots", "petit bot", " all ", "all:", "all!", "you all", "vous", "touti", "tutti", "la compagnie", "la jeunesse", "les nanceens", "les nanceiens", "la Lorraine", "les lorrains", "everybody", "'vrybody", --[[SURNOM, ]] "ach'", "anybody", " there", "folks", "boys", "#nancy", "#la-bas", "messieurs", "everyone", "les potos", "les cocos", "mon biquet", "les amis", "les z'amis", "les copains", "les aminches", --[["bande de ") && ! ILS_FONT ]] "m'sieurs dame", "m'sieurs-dame", "messieurs-dame", "messieurs dame", "messieurs", "les n'enfants", "les enfants", "les zenfants", "les petits", "le people", "le peuple", "la foule", "todos" --[[ tous en espagnol]]},
	 "NOM"},

	{{"y a", "y'a", "y-a", " est la", "there"},
	 "ILYA"},

	{{"bonjour", "bonne journee", "bjr", "'jour", "hello", "hola" --[[ En espagnol]], {"lo ", "^rigolo"},  "szia" --[[ En hongrois ]], "hei " --[[ En finnois ]],  {"hej", "^hej da" --[[ En suedois ]]}, "salam" --[[ En arabe ]], "zdravi" --[[ En slovaque ]]},
		  "BONJOUR"},

	   {{"salu",
		 {"hi ", "^high", "^hihi","^chi"},
		 "coucou", "kikou", "youhou", "heya", "beuha", "yo ", "hugh", "'lut", "lut ", "slt", "hallo" --[[ En allemand ]], "tjena" --[[ En suedois ]]},
		 "SALUT"},

		{{"je reste"},
		 "JE_RESTE"},

		{{"ça va", "ca va", "ca gaz", "vas-tu", "vas tu", "vas bien", "va bien", "en forme", "le moral", "la forme", "comment va", "comment tu va", "comment te sens-tu", "comment te sens tu", "comment tu te sens", "comment tu t'sens", "ti k'sa va", "ca baigne", "ca boume", "ca roule", "sens bien", "vous allez bien", "allez-vous bien", "comment vous allez", "comment allez-vous", "commen y le" --[[en creole]] , "how are u", "how are you", "how do you do", "Mita kuuluu" --[[en finnois]], "tu rak bikir" --[[en arabe]]},
			  "CAVA"},

	  {{"au revoir", "arvoir", "arrosoir persi", "a+", "a plus", "m'en vais", "a bientot", {"bon week end", "^passe un bon"}, "bon we", "bon week-end", "m'en aller", " partir", "vais y aller", "vais vous laisser", "y vais", "bye", "a demain", "a 2 mains", "adtaleur", "ad'taleur", "a tout-a-l'heure", "a tout a l'heure", "a tout a l heure", "a tt a l'heure", "nar r'trouv" --[[ Creole ]],"n'artrouv", "n'ar trouv", " me casse", " m'casse", "vous laisse", "je te laisse", "a un de ces quatres", "a un de ces 4", "a un de c 4", "a 1 de c 4", "a12c4", "a1274", "see you", "see u", "see ya", "later", "c u ", "je pars", "je parte", "tchao", "tshaw", "ciao", "j'y aille", "me sauve", "hasta luego", "arwar", "a la prochaine", "vi ses" --[[ En suedois : "on se voit" ]],"cassos", {"je rentre","^quand "},"revoyure", "r'voyure", "wiedersehen", "wieder sehen", "have to go", "I must go", "time to go", "nar trouver zot tout" --[[ En Creole ]],"hej da" --[[En suedois ]]},
			"AUREVOIR"},

	{{"je m'en vais", "j'm'en vais", 
	  function (from,to,msg) return (chaine_est_dans(msg,"je vais") or chaine_est_dans(msg, "je v ") or chaine_est_dans(msg, "j'vais") or chaine_est_dans(msg, "jvai")) and (chaine_est_dans(msg, " y aller") or chaine_est_dans(msg, " m'en aller") or chaine_est_dans(msg, " partir") or chaine_est_dans(msg, "rentrer")) end,
	  "que j'y aille", "je pars", "j'y vais", "j'y go", "j y go", "j go", "je rentre", "cassos", "je me casse", "j'me casse", "je me barre", "j'me barre", "je me sauve", "j'me sauve", "je me rentre", "j'me rentre", "je reviendrai", "je dois y aller", "je dois partir", "je dois filer", "je vous quitte", "j'vous quitte", "I've to leave"},
	 "JE_M_EN_VAIS"},

   {{{"merci", "^commerci"},"merchi","'ci","thanks","thanx","thx",{"tack","^tackl" --[[ En suedois ]]}},
	"MERCI"},

	{{"bonne annee", "happy new year"},
	 "BONNE_ANNEE"},

	{{"connard", "couillon", "ducon", "con ", {"con,", "^facon", "^rcon", "^scon", "^conf", "^falcon"}, "gros con", "sale con", "tit con", " des cons", "salaud", {"salop", "^salopette"}, "mechant", "injuste", "degueulasse", "malpropre", --[[{"sale", "^salem", "^ssale"]] "tu pues", " puent", "pourceau", "tu sens movais", "tu sens mauvais", "sentent mauvais", "amibe", "amorphe", "leche-cul", "leche cul", "casse-couille", {"pourri", "^pourriez"}, {"tache", "^moustache", "^pistache"},"raclure", "indecent", "vulgaire", "triple buse", "chacal", "primate", "rascal", "minable", "corniaud", "trisomique", "imbecile", "baka" --[[ imbecile en japonais ]],"corniaud", "blaireau", "cafard", "idiot", "stupid", "nigaud", "neuneu", " bete", "faible d'esprit", "attarde mental", "cretin", "debile", "niais", "nul ", {" nul", "^match"},"abruti", "ahuri", "gogol", "naze", "pignouf", "mauvais", "mongole", "andouille", "cornichon", "banane", "quetsche", "cake", "enfoire", "encule", "enqule" --[[ Y'en a qui sont nuls en orthographe! ]],{"fuck","^!fuck"}, "va te faire", "te faire foutre", "lopete", {"lopette", "^salopette"},"tapette", {"pd", "^pdt", "^repdel"},"tantouse", "impuissant", "putain", "putin" --[[ faute d'ortho ]],"bitch", "tu suces", "tete de noeud", "fumier", "trou du c", "trou duc", "trouduc", "chiant", {"fais chier", "^te fais chier"},	{"fait chier", "^se fait chier"},"vas chier", " de daube", "charogne", "lourd", {"merd", "^demerd"},"espece d", "ta gueule", "ta geule" --[[ Faute d'orthographe courante ]], "face de ", "menteur", "malhonnete", "escroc", "charlatan", "canaille", "chiendent", {"patate", "patater"}, {"outre", "^foutre"},"bouffon", --[["pelo") ?!? ]]"pov gars", "moche", "affreu", "laid", "vilain", "morveux", "alcolo", "alcoolo", "alcoolique", "alcolique", "navrant", "lamentable", "faineant", "faignant", "flemmard", "bouzeu", "bouseu", "sagouin", "sagoin"},
	"INJURE"},

	 {{"intelligent", {"demerde", " bien"}, "courageux", {"cool", "^alcool"}, "sympa", "marrant", "rigolo", "joyeux", {" poli", "^polisson"}, " fort ", " forts ", {"puissant", "^impuissant"}, "efficace", "rapid", "champion", "super", "extra", "genial", "geniaux", "genie", "une bete", "etalon", "le meilleur", "excellent", --[[{"juste", "^injuste"},]]"interessant", "impressionnant", "epatant", {"beau", "^beauc", "^beaut"}, "elegant", "galant", "magnifi", "splendide", "joli", "classe", "merveille", "chouette", " chou", "gentil", "mignon", "adorable", "adore", "bravo", "un bon", "tres bon", "bon bot", "bien vu", "serviable", "felicit", "un ange", "un dieu"},
	"COMPLIMENT"},

	{{"parl", "discut", "bavarde", {"cause", "^because", "^a cause "}, "speak", "talk", "prata" --[[ En suedois ]], "habla" --[[ En espagnol ]]},
	"PARLER"},

	{{"english", "anglais", "angliche", "american", "eng."},
	 "ENGLISH"},

	{{"french", "frog", "francais"},
	 "FRENCH"},

	{{"svenska", "suedois", "swedish"},
	 "SWEDISH"},

	{{"areuh", "gouzi", "gazou"},
	 "BEBE"},

	{{"tout seul", "toute seule", "alone", {"personne", function (from,to,msg) return Flags.ILYA end}},
	 "TOUTSEUL"},

	{{"nancy"},
	 "NANCY"},

	{{"parent", "pere", "papa", " mere", "maman"},
	 "PARENTS"},

	{{{" ou ", "^ ou quoi"}, "d'ou", {" ou", "^ oui", "^ ou quoi"}, "quel endroit", "where", "from", "ou ca", "quel coin"},
	 "QUEL_ENDROIT"},

	{{"ennui", "ennuy", {"chier", "^fichier"}, "embet", "mortel"},
	 "ENNUI"},

	{{"dors ", " dort ", "dorm", "lethargie", "sleep", "reveille", "roupill"},
	 "DORMIR"},

   {{"reveille toi", "tu te reveille"},
	"REVEILLE_TOI"},

   {{ "quelqu'un", "quelqu un", "kelk'un", "kelkun", "kkun", "kk1", "kelkin", "qquun", "qq'un", "qqun", "qqn", "qq1", "qlqn", " des gens", "someone", "somebody", "somebady", "any", "jemand", "alguen" --[[ En espagnol ]]},
	"QUELQUUN"},

	{{"calme", "mort", {" quiet", "^inquiet"}, --[["un ange passe",]] "tranquille", "reposant"},
	 "CALME"},

	{{"ta gueule", "ta geule" --[[ faute courante ]], "shut up", {"chut", "^chute", "^chuti"}, "silence", "tais-toi", "tais toi", "te taire", "ferme la", "la ferme", "boucle la", "ca suffit", "en veilleuse"},
		  "TAISTOI"},

	{{"bonsoir", "bonne soiree", "bonne nuit", "beuah" --[[ En Guignols ]], "dobru noc" --[[ En slovaque ]]},
		  "BONSOIR"},

	{{{"aper", "^taper"}, "bon appe", "bonne appe" --[[ Pfff! ]], "bonne bouffe", "suze", "ricard"},
		  "BONAPP"}, --[[ TODO DOUBLON SUR APER ??]]

	{{"dejeuner", "diner", "manger", "grailler", "bouffer"},
	 "REPAS"},

	{{"je pars ", "vais aller", "je vais", "je v ", "je file"},
	 "JEMENVAIS"}, --[[ TODO mixer REPAS et JEMENVAIS comme c'était dans le code C -> VAIS_MANGER ]]

	{{"enerve"},
	 "ENERVEMENT"},

	{{"bisou", "bizou", "bise", "poutou", "embrass", "smack"},
	 "BISOUS"},

	{{"boire", "apero", "champagne", "pommard", "pastis", "buvons", " vin "},
	 "ABOIRE"},

	{{" moi ", "je ", "j'", " me "},
	 "MOI"},

	{{"pelforth", "Guinness", "Guiness" --[[ erreur courante ]], "Kro", "K-net", "canette", "binouse", " pel ", " mousse", "biere"},
	 "BIERE"},

	{{{"pas vrai", "^pas vraiment"}, "n'est-ce-pas", "n'est-ce pas", "n'est ce pas", "n est ce pas", "hein ", "hein,", "isn't it"},
		"PASVRAI"},

	{{function (from,to,msg) 
		 return ((chaine_est_dans(msg, " pas ") or chaine_est_dans(msg, " po ")) and not(Flags.PASVRAI)
	  or chaine_est_dans(msg, " ne ") or chaine_est_dans(msg, "sauf")
      or (chaine_est_dans(msg, " n'") and not(chaine_est_dans(msg, " qu'") or chaine_est_dans(msg, " que ")))
   and not(chaine_est_dans(msg, "un peu")) and not(Flags.PASVRAI))
	  end},
	 "NEGATION"},

	{{"d'accord", "d accord"},
	 "DACCORD"},

	{{"excuse", "pardon", "desole", "pitie", "supplie"},
	 "EXCUSE"},

	{{"repond", "reponse"},
	 "REPONDRE"},

	{{{"ami", "^gamin", "^amiga", "^!whoami"}, "copain", "camarade", "pote", "poto"},
	 "AMI"},

	{{{"heure", "^heureu", "^tout-a-l'heure", "^tout a l'heure"},
	  {"time", "^stime", "^timent", "^!time"}},
	 "HEURE"},

--[[
	TODO : a revoir
  if (ChaineEstDans (msg, "faim") ||
      ChaineEstDans (msg, "manger") &&
      (ChaineEstDans (msg, "veux") || ChaineEstDans (msg, "envie d"))
      )
    FAIM       = TRUE;
]]

	{{"soif"},
	 "SOIF"},

	{{"pense"},
	 "PENSER"},

	{{"es-tu ", " es-tu", "t'es ", "es tu", "est tu" --[[ orthographe! ]], "t es ", "tu es ", "vous etes", "etes vous", "etes-vous", "are you ", "r u "},
		  "ES_TU"},

	{{"t'es ", "t es ", "tu es ", "vous etes", "you are", "u r "},
	 "TU_ES"},

	{{"tu as ", "t'as ", "t as ", "vous avez"},
	 "TU_AS"},

	{{"un bot", "un robot", "un automate", "un programme", "androide", "androïde", "une machine"},
	 "UN_BOT"},

	{{"un mec", "un gars", "garcon", "homme"},
	 "UN_MEC"},

	{{"humain", "homme", "homo sapiens"},
	 "HUMAIN"},

	{{"fille", "une nana", "une femme", "une meuf"},
	 "UNE_FILLE"},

	{{"malade", "pas bien", "pas en forme", "souffrant"},
	 "MALADE"},

	{{"stp", "s'il-te-plait", "s'il te plait", "s il te plait", "svp", "s'il-vous-plait", "s'il vous plait", "s il vous plait", "please"},
	 "STP"},

	{{"quel age", "kel age"},
	 "AGE"},

	{{":)", ":-)", ":D", ":-D", ":=)", ":=D", "^_^"},
	 "SOURIRE"}, -- TODO a verifer, les : ne passent pas tous

	{{{"arf", "^parfait", "^parfois"}, "ouah", "ouaf", "woof", "wouf", "waf", "bow wow" --[[ en anglais ]], "grr"},
	"MOUARF"},

	{{function (from,to,msg)
		 if Flags.MOUARF and (chaine_est_dans(BotNick, "arf") and not(chaine_est_dans(BotNick, "parfait")) and not(chaine_est_dans(BotNick, "parfois")))
		 or chaine_est_dans(BotNick, "ouah")
		  or chaine_est_dans(BotNick, "ouaf") 
		   or chaine_est_dans(BotNick, "woof")
			or chaine_est_dans(BotNick, "wouf")
			or chaine_est_dans(BotNick, "waf")
			or chaine_est_dans(BotNick, "grr") then
			Flags.MOUARF = false
		 end
	  end},
	 "MOUARF"},

	{{";)", ";D", ";-)", ";-D", "-_^", "^_-"},
	 "CLINDOEIL"},

	{{"et la mer a la ronde roule son bruit de cranes sur les greves"},
	 "EXTRAITSTJOHNPERSE"},

	{{"Saint John Perse"},
	 "STJOHNPERSE"},


	{{" la?", "ici", " la,", "present", "avec nous"},
	 "PRESENT"},

--[[  if (/* ChaineEstDans (msg, "@>---,--`--") ||
      ChaineEstDans (msg, "@>---,---`--") ||
      ChaineEstDans (msg, "@>--,---`--") ||
      ChaineEstDans (msg, "@}--`--,--") */
]]
	{{"@>--", "@}--", "--<@", "--{@", "@-", "-@"},
	 "ROSE"},

	{{" mort"},
	 "MORT"},

	{{"indiii", "lililililili", "bisous bisous bisous"},
	 "INDILILI"},

	{{"et toi", "toi aussi"},
	 "ETTOI"},

	{{"je suis", "j'suis ", "chuis ", "suis-je", "suis je", "moi "},
	 "JESUIS"},

	{{"es-tu", "as-tu", "est-ce", "est-il", "comment", "pourquoi", "suis-je", "ais-tu", "ait-il", "ez-vous", "?"},
	 "QUESTION"},

	{{"as-tu ", " as-tu", "as tu ", " as tu ", "t'as", "t as", "tu as", function (from,to,msg) return Flags.TU_AS and Flags.QUESTION end},
	 "AS_TU"},

	{{{"fou", "^four"}, "tare", "cogne", "schtarbe", "toque", "cingle", "dingue", "secoue", "s'coue", "maboul", "toc toc", "psychopathe", "psychopate", "psychopatte"},
	 "FOU"},

	{{"musique", " chant", "ploum ploum", "pom pom pom"},
	 "MUSIQUE"},

	{{"ecoute"},
	 "ECOUTER"},

	{{"copine", "fiancee", "petite amie"},
	 "COPINE"},

	{{"pirate", "hacker"},
	 "PIRATE"},

	{{"warez"},
	 "WAREZ"},

	{{"bienvenu", "bon retour", "content de te revoir", "welcome", "valkommen" --[[ en suedois ]]},
		  "BIENVENUE"},

   {{"grand", "geant"},
	"GRAND"},

   {{"petit", "nain"},
	"PETIT"},

   {{"libre", {"disponible", "^indisponible"}},
	"LIBRE"},

   {{"occupe"},
	"OCCUPE"},

   {{"ce soir", "cette nuit", "tonight"},
	"CE_SOIR"},

   {{"aimes-tu ", "aimes tu ", "tu aimes", "aimez-vous ", "aimez-vous", "t'aimes "},
	"AIMES_TU"},

   {{"m'aimes-tu", "m aimes tu ", "m'aimes tu ", "tu m'aimes", "m'aimez-vous "},
	"M_AIMES_TU"},

   {{"connais-tu", "connais tu ", "tu connais", "connaissez-vous ", "connaissez vous ", "connaitrais-tu ", "connaitrais tu ", "apprecies-tu ", "apprecies tu "},
	"CONNAIS_TU"},

   {{"neige", "flocon"},
	"NEIGE"},

   {{"qu'est-ce qu", "c'est quoi ", "c est quoi ", "c quoi", "c koa", "qu'est ce qu", "qu est ce qu", "kesako"},
	"QUEST_CE"},

   {{"habite", "creche", "loge", "reside", "vis chez", "es-tu de ", "es tu de ", "es-tu en ", "es tu en ", "reste tu" --[[ Quebequois ]], "restes tu", "restes-tu"},
	"HABITE"},

	{{"france"},
	 "FRANCE"},

	{{"c'est", "c est"},
	 "C_EST"},

	{{"loria"},
	 "LORIA"},

	{{"talon"},
	 "TALON"},

	{{"ooooooooorrrrrrrr", "roooooote", "BOAAAARP", "burp"},
	 "ROT"},

	{{"pourquoi", "pourkoi", "pkoi", "pq ", "why", "warum"},
	 "POURQUOI"},

	{{"tu crois", "crois tu", "crois-tu"},
	 "CROIS_TU"},

	{{"sais-tu", {"sais tu", "^ssais"}, "tu sais", {"t'sais", "^saisi"}},
	 "SAIS_TU"},

	{{"fais-tu", "fais tu", "tu fais", "sais-tu faire", "tu sais faire"},
	 "FAIS_TU"}, --[[ TODO SAIS_TU = FALSE]]

	{{"cafe", "kawa"},
	 "CAFE"},

	{{"repondre", "reponds", "reponse"},
	 "REPONDS"},

	{{"sourd", "entends rien"},
	 "SOURD"},

	{{"rien", "compri", "comprend", "pige"},
	 "RIENCOMPRIS"},

	{{"fatigue", "creve", "vanne", "a plat", "dans l'gaz", "ereinte", "extenue"},
	 "FATIGUE"},

	{{"je t'aime", "je t aime", "j't'aime", "je vous aime", "je vous adore", "je t'adore", "je t adore", "j't'adore"},
	 "JE_T_AIME"},

	{{"a tes souhaits", "a tes amours"},
	 "SOUHAIT"},

	{{{"veux", "^cheveux"}, "ordonne", function (from,to,msg) return Flags.MOI end},
	 "VEUX"}, --[[ TODO verifier ]]

	{{"crin", "Centre de Recherche en Informatique de Nancy"},
	 "CRIN"},

	{{"arabe", "beur", "negr", "noirs", "bougnoul", "niakoue", "bico"},
	 "ETRANGERS"},

	{{"qui ", "qui?", "qui,"},
	 "QUI"},

	{{"tu fais", "fais-tu", "fais tu"},
	 "TU_FAIS"},

	{{{"quoi", "^pourquoi"}},
	 "QUOI"},

	{{"vive", "viva", "rules", "rulez", "rulz"},
	 "VIVE"},

	{{"amiga", "68000", "motorola"},
	 "AMIGA"},

	{{{"intel", "^intelligent"}, "Windows", "Microsoft"},
	 "WINTEL"},

	{{"front national", "f.n.", {" fn", "^fnac"}},
	 "FRONT_NATIONAL"},

	{{{"merde", "^demerde"}, {"shit", "^!shit", "^shitt"}, "couill", "connard", "ducon", "zob", {"bite", "^abite"}, "salaud", {"salop", "^salopette"}, "degueu", "blair", "pignouf", "enfoire", "encule", {"pd", "^pdt"}, " pd ", {"fuck","^!fuck"}, "fuk", "branler", "putain", "pt1", "putin", "bordel", {"chier", "^ichier"}, "chiant", "gueule", "dick", "suck", "sux", "tu suce", "bitch", "bougnoul", " de daube", "leche-cul", "leche cul", {"culs", "^calcul"}, "mon cul", "trou du c", "trou duc", "trouduc", "asshole", "foutre", {"niqu", "^pique"}, "pisse", "bougnoul", "niakoue", "bico"},
	 "GROS_MOT"}, --[[ TODO voir avec INJURE si ça ne se recooupe pas ]]

	{{{"pour", "^pourceau", "^pourri"}},
	 "POUR"},

	{{"contre"},
	 "CONTRE"},

	{{"boude", "fais la gueule", "fait la gueule", "fais la tete", "fait la tete"},
	 "TU_BOUDES"},

	{{"liste", "montre", "donne", "stimuli"},
	 "LISTE_STIMULI"},

	{{"teste stimuli", "test stimuli", "tester stimuli"},
	 "TESTE_STIMULI"},
  
	{{function (from,to,msg) return (chaine_est_dans(msg, "liste") or chaine_est_dans(msg, "montre") or chaine_est_dans(msg, "donne")) and chaine_est_dans(msg, "reponse") end},
	 "LISTE_REPONSES"},

	{{"sacre"},
	 "SACRE"},

	{{"jaloux"},
	 "JALOUX"},

	{{"qui est", "Qui c'est", "C'est qui", "c qui", "c ki", "c'ki"},
	 "QUI_EST"},

	{{"ton maitre", "ton maître", "ton seigneur", "ton chef"},
	 "TON_MAITRE"},

	{{"raison"},
	 "RAISON"},

	{{"kicker", "kiker"},
	 "KICKER"},

	{{"kicke moi", "tu me vire", "tu me gicle"},
	 "KICKE_MOI"},

	{{"ciseau", "cisaille"},
	 "CISEAUX"},

	{{"papier", "feuille"},
	 "PAPIER"},

	{{"caillou", "pierre", "rocher"},
	 "CAILLOU"},

	{{function (from,to,msg) return Flags.PAPIER and Flags.CAILLOU and Flags.CISEAUX end},
	 "PIERRE_PAPIER_CISEAUX"},

--[[
	TODO 
  if (PIERRE_PAPIER_CISEAUX) {
    PAPIER = FALSE;
    CAILLOU = FALSE;
    CISEAUX = FALSE;
  }
]]

	{{"tricheur", "tu triche", "tu as trich", "tu a trich", "t'as trich", "t'a trich"},
	 "TRICHEUR"},

	{{"mauvais joueur"},
	 "MAUVAIS_JOUEUR"},

	{{":~(", ":'(", ":'C", ":'O", ":_(", " ouin ", " ouinnnn", "wouinnn"},
	 "PLEURER"},

	{{"modeste"},
	 "MODESTE"},

	{{"content"},
	 "CONTENT"},

	{{"je te deteste", "je te hais"},
	 "JE_TE_DETESTE"},

	{{"achille"},
	 "ACHILLE"},

	{{"!say", {"say", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_SAY"},

	{{"!fuck", {"fuck", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_FUCK"},

	{{"!apprends", {"apprends", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_APPRENDS"},

	{{"!desactive", {"desactive", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_DESACTIVE"},

	{{"!active", {"active", "^desactive", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_ACTIVE"},

	{{"!join", {"join", function (from,to,msg) return not(is_channel(to)) end}},
	 "FUNCTION_JOIN"},

--[[ TODO
  if (FONCTION_APPRENDS || FONCTION_FUCK || FONCTION_SAY ||
      ChaineEstDans (msg, "!active") ||
      !ischannel (to) && ChaineEstDans (msg, "active") ||
      ChaineEstDans (msg, "!desactive") ||
      ChaineEstDans (msg, "!join") ||
      !ischannel (to) && ChaineEstDans (msg, "join"))
    FONCTION = TRUE;
]]

}

Reactions = {
   {{"SBOUB"},
	{";o)", "sboub alors !", "magne toi le sboub !", "on danse une sboubada ?"},
	0,
	{},
	0},

--   {{"SBOUB"},
--	{";o)", "sboub alors !", "magne toi le sboub !", "on danse une sboubada ?"},
--	0,
--	{},
--	0},
--
}


function ProcessFlags(from, to, msg)
   -- check for the predefined flags 
   --local Triggers = Triggers
   Flags = {}
   print(#Triggers .. " déclencheurs")
   for kt,vt in pairs(Triggers) do
	  --vt est un déclencheur vt[1] = table des mots, vt[2] = nom
	  local liste = vt[1]
	  for kp,vp in pairs(liste) do
		 --vp est une pattern pour un déclencheur
		 if type(vp) ==  "string" then --pattern simple
			if chaine_est_dans(msg, vp) then 
			   Flags[vt[2]] = true
			   break
			end
		 elseif type(vp) == "table" then --pour les cas "x mais pas y ni z"
			--print("table de "..#vp.." eléments trouvée pour tester le declencheur " .. vt[2])
			res = true
			for key,val in pairs(vp) do
			   if type(val) == "string" then
				  if string.sub(val,1,1) == "^" then
					 --print("ne doit pas contenir "..string.sub(val,2))
					 res = res and not(chaine_est_dans(msg, string.sub(val,2)))
				  else
					 --print("doit contenir "..val)
					 res = res and chaine_est_dans(msg, val)
				  end
			   elseif type(val) == "function" then --pour les cas difficiles, on peut utiliser une fonction
				  --print("appel d'une fonction ")
				  res = res and val(from, to, msg)
			   elseif type(val) == "boolean" then
				  --print("test bool "..val)
				  res = res and val
			   else 
				  print("type non gere pour test de "..vt[2].." : "..type(val))
			   end
			end 
			if res then 
			   Flags[vt[2]] = true
			   break
			end
		 elseif type(vp) == "function" then --pour les cas difficiles, on peut utiliser une fonction
			--print("appel d'une fonction ")
			if vp(from, to, msg) then
			   Flags[vt[2]] = true
			   break
			end
		 end
	  end
   end
end 

function ProcessReponsesSimples(from, to)
   for kr,vr in pairs(Reactions) do
	  --vr est une réaction
	  
	  --vr[1] est la liste des flags
	  --vr[2] est la liste des réponses positives
	  --vr[3] est l'humeur positive
	  --vr[4] est la liste des réponses négatives
	  --vr[5] est l'humeur négative
	  local flags = vr[1]
	  for kf,vf in pairs(flags) do
		 if Flags[vf] then
			repondre(from, to, vr[3], vr[2], vr[5], vr[4])
			if Flags.STP then
			   repondre(from, to, 2, nil, 3, nil)
		    end 
	  		break
		 end
	  end
   end
end

function TraiteMessage(from, to, message, nbphrases)

   print(string.format("Message de %s -> %s : '%s'", from, to, message))

   tdeb = os.clock()
   ProcessFlags(from, to, message)

   --ajustements
   --local Flags = Flags   
   if Flags.NEGATION then
	  if Flags.LIBRE then
		 Flags.LIBRE = false
		 if NOT(Flags.OCCUPE) then 
			Flags.OCCUPE = true
		 end
	  elseif Flags.OCCUPE then
		 Flags.OCCUPE = false
		 if NOT(Flags.LIBRE) then 
			Flags.LIBRE = true
		 end
	  end

	  if Flags.JE_T_AIME then
		 Flags.JE_T_AIME = false
	  end

	  if Flags.ABOIRE then
		 Flags.ABOIRE = false
	  end
	  if Flags.ENERVEMENT then
		 Flags.ENERVEMENT = false
	  end
	  if Flags.AUREVOIR then Flags.AUREVOIR = false end
	  if Flags.MERCI then Flags.MERCI = false end
	  if Flags.INJURE then
		 Flags.INJURE = false
		 Flags.COMPLIMENT = true
	  elseif Flags.COMPLIMENT then
		 Flags.COMPLIMENT = false
		 Flags.INJURE     = true
	  end

	  if Flags.TRICHEUR then
		 Flags.TRICHEUR = false
		 Flags.COMPLIMENT = true
	  end

	  if Flags.MAUVAIS_JOUEUR then
		 Flags.MAUVAIS_JOUEUR = false
		 Flags.COMPLIMENT = true
	  end

	  if Flags.PARLER then
		 if Flags.FRENCH then
			Flags.ENGLISH = true
			Flags.FRENCH = false
		 elseif Flags.ENGLISH then
			Flags.FRENCH  = true
			Flags.ENGLISH = false
		 end
	  end
	  if Flags.TOUTSEUL then
		 Flags.TOUTSEUL = false
		 Flags.QUELQUUN = true
	  end
	  if Flags.ENERVEMENT then Flags.ENERVEMENT = false end
	  if Flags.BISOUS then Flags.BISOUS = false end
	  if Flags.DACCORD then Flags.DACCORD = false end
	  if Flags.EXCUSE then Flags.EXCUSE = false end
	  if Flags.AMI then Flags.AMI = false end
	  if Flags.GRAND then
		 Flags.GRAND = false
		 Flags.PETIT = true
	  elseif Flags.PETIT then
		 Flags.GRAND = true
		 Flags.PETIT = false
	  end
	  if Flags.CALME then
		 Flags.CALME = false
	  end
	  if Flags.SALUT then
		 Flags.SALUT = false
	  end

	  if Flags.POUR then
		 Flags.CONTRE = not(Flags.CONTRE)
		 Flags.POUR = false
	  elseif Flags.CONTRE then
		 Flags.CONTRE = false
		 Flags.POUR = not(Flags.POUR)
	  end
	  
	  if Flags.MODESTE then
		 Flags.MODESTE = false
	  end

	  if Flags.CONTENT then
		 Flags.CONTENT = false
	  end
   end   
   if chaine_est_dans(message, "il fait") or chaine_est_dans(message, "temps") then
	  if Flags.INJURE then
		 Flags.INJURE = false
	  end
	  if Flags.COMPLIMENT then
		 Flags.COMPLIMENT = false
	  end 
   end

   Flags.LANGUE = Flags.FRENCH or Flags.ENGLISH or Flags.SWEDISH

   Flags.FONCTION = Flags.FONCTION_SAY or Flags.FONCTION_FUCK or Flags.FONCTION_JOIN or Flags.FONCTION_ACTIVE or Flags.FONCTION_DESACTIVE or Flags.FONCTION_APPRENDS

   tfin = os.clock()
   print(string.format("elapsed time: %.2f\n", tfin-tdeb))

   print("liste des flags")
   for k,v in pairs(Flags) do if v then print(k) end end

   isChannel = is_channel(to)

   if (isChannel and is_log_on(to)) or (not isChannel and LogLevel > 0) then
      botlog("bot.log", "<%s#%s> %s", from, to, message .. "<----- from lua")
   end

   if Flags.FONCTION then
	  if isChannel then
		 AncienneAutorisation = can_talk(to)
		 set_talk(to, false)
	  else
		 AncienneAutorisation = true
	  end
   end
   
   if isChannel then
	  canTalk = can_talk(to)
   else
	  canTalk = true
   end

   if canTalk then
	  -- on a l'autorisation de parler

	  Locuteur = locuteur_existe(from)
	  --print("locuteur_existe(" .. from .. ") a retourné " .. tostring(Locuteur))
	  if Locuteur == 0 then
		 Locuteur = ajoute_locuteur(from)
		 --print("ajoute_locuteur(" .. from .. ") a retourné " .. tostring(Locuteur))
	  end
	  --print("locuteur_bonjours(" .. tostring(Locuteur) .. ") a retourné " .. locuteur_bonjours(Locuteur))

	  ProcessReponsesSimples(from, to)

	  --process stimulis simples
	  --(fichiers stims /reps)

	  --

	  if Flags.STP then repondre(from, to, 2, {}, 3, {}) end -- on remonte en étant poli :o)

	  LocBonjours = locuteur_bonjours(Locuteur)

	  --Bonjour
	  if (Flags.BONJOUR and (Flags.NOM or (not isChannel))) and LocBonjours <= 0 then
		 repondre(from, to, 
				  1, {"Bonjour %s.", "Bonjour à toi %s.", "Je te souhaite le bonjour, %s.", "Bien le bonjour, %s.", "Bonne journée à toi aussi, %s.", "Bonjour à toi aussi, %s.", "Bonjour, comment vas-tu aujourd'hui, %s?", "/me enlève son chapeau devant %s."},
				  1, {"Au revoir, %s.", "%s: Pfff...", "", "Tu ne le penses même pas, %s!", "Fais pas chier, %s!", "/me tourne le dos à %s."})
		 locuteur_setbonjours(Locuteur, 1)
	  end 

	  --Bonsoir
	  if (Flags.BONSOIR and (Flags.NOM or not isChannel)) and LocBonjours >= 0 then
		 repondre(from, to, 
				  1, {"Bonsoir %s.", "Bonsoir à toi %s.", "Je te souhaite le bonsoir, %s.", "Bien le bonsoir, %s.", "Bonne soirée à toi aussi, %s.", "Bonsoir à toi aussi %s.", "/me souhaite le bonsoir à %s."},
				  1, {"C'est ça, casse toi %s.", "%s: Pfff...", "Barre-toi, %s.", "Bon débarras, %s!", "Un casse-pieds de moins!", "/me soupire d'aise: %s est parti."})
		 locuteur_setbonjours(Locuteur, -1);
	  end

	  --12h depuis dernier contact ?
	  print ("test dernier contact > 12h : now =" .. os.time())
	  print ("test dernier contact > 12h : nowdernier contact =" .. locuteur_derniercontact(Locuteur))
	  datedif = (os.time() - locuteur_derniercontact(Locuteur))
	  print ("test dernier contact > 12h : now - dernier contact =" .. datedif)
	  if (LocBonjours > 0) and ((datedif / 3600) > 12) then
		 locuteur_setbonjours(Locuteur, 0)
	  end
	  
	  LocBonjours = locuteur_bonjours(Locuteur)
	  if Flags.SALUT and (Flags.NOM or not isChannel) and (LocBonjours <= 1) and (LocBonjours >= 0 ) then
		 if LocBonjours > 0 then
			salut = "Au revoir, %s."
		 else
			salut = "Bien le bonjour, %s."
		 end
		 repondre(from, to,
				  1, {"Salut %s!", "Je te salue, %s!", "Je te salue bien bas, %s!", "Je te rends un salut, %s (mais pas le tien ;)", "Salut %s.", "Salut à toi aussi, %s.", salut, "/me salue %s.", "/me serre la main de %s.", "Salut %s. :-)"},
				  1, {"J'te cause plus %s!", "Je ne vous salue pas, %s!", "Hé bien moi, je ne te salue pas, %s!", "Barre-toi, %s.", "Zut, un casse-pieds (%s)!", "/me se tourne de manière à ne plus voir %s."})
		 
		 if LocBonjours > 0 then 
			locuteur_setbonjours(Locuteur, 0)
		 else
			locuteur_setbonjours(Locuteur, LocBonjours + 1)
		 end
		 LocBonjours = locuteur_bonjours(Locuteur)
	  end
	  
	  --je reste
	  if Flags.JE_RESTE and (Flags.NOM or not isChannel) then
		 if locuteur_bonjours(Locuteur) < 0 then
			repondre(from, to,
					 1, {"Excuse moi %s, j'avais pas compris.", "%s: Oups. Je n'ai pas fait assez attention.", "Tant mieux %s.", "J'ai cru que tu partais, %s.", "Je préfères que tu sois là, %s.", "Je me sens moins seul quand tu es là, %s.", "J'ai sans doute mal compris, %s.", "J'espère que tu restes longtemps, %s.", "Super, %s."},
					 1, {"%s: Zut!", "Tant pis, %s!", "Grmbl..."})
		 end 
	  end

	  --Ça va ?
	  if Flags.CAVA and (Flags.NOM or not isChannel) then
		 repondre(from, to, 
				  1, {"Ça va pas mal, %s, merci.", "Moui, ça va, %s, merci.", "On fait aller, %s.", "Ça peut aller, %s.", "Ça allait très mal, mais ça commence à aller mieux, %s, merci.", "Depuis que tu es là, %s, ça va mieux.", "Ça va mieux, merci %s.", "Ça va bien, et toi %s?", "Ça va très bien, %s.", "Tout est pour le mieux dans le meilleur des mondes, %s.", "Je suis sur un petit nuage, %s. :)", "J'ai la pêche, %s!"},
				  1, {"J'espère que je vais mieux que toi, %s!", "Ça va déjà moins bien depuis que tu es là, %s!", "Ça ira mieux quand tu seras parti, %s.", "Bof, ça va pas fort, %s.", "Ça va très mal, %s.", "Je déprime depuis que tu es arrivé, %s...", "J'ai la banane, pas comme toi, %s! :]", "/me soupire."})
	  end

	  --au revoir
	  if Flags.AUREVOIR and (Flags.NOM or not isChannel) and (locuteur_bonjours(Locuteur) >= 0) then
		 repondre(from, to,
				  1, {"À bientôt %s.", "Au revoir %s.", "Au plaisir de te revoir %s.", "À plus tard %s.", "Reviens-nous bientôt, %s.", "À bientôt, %s.", "À tantôt, %s.", "À la revoyure, %s.", "Bye bye %s.", "Je ne te raccompagne pas, %s: tu connais le chemin. :)", "/me décroche le manteau de %s du porte-manteau et l'aide à l'enfiler."}, 
				  1, {"En espérant ne plus te revoir, %s.", "%s: Bon débarras!", "Au plaisir de ne plus te revoir %s.", "Débarrasse le plancher, %s!", "Du balai, %s!", "/me attend que %s soit sorti avant de claquer la porte."})
		 locuteur_setbonjours(Locuteur, -1)
	  end
	  
	  --injure
	  if Flags.INJURE and not Flags.COMPLIMENT and not Flags.ES_TU and not Flags.QUI_EST and not Flags.ETTOI and not Flags.JESUIS and not Flags.CLINDOEIL and not Flags.FONCTION_FUCK and (Flags.NOM or not isChannel) then
		 if not exist_userhost(from) then
			add_to_levellist(from)
			Nouveau = true
		 else
			Nouveau = false
		 end
		 if Nouveau then
			add_to_level(from, 3)
		 else
			add_to_level(from, -2)
		 end
			
		 repondre_kicker(from, to,
						 -2, {"Attention, %s, je vais kicker! J'exige des excuses.", "Implore mon pardon, misérable %s!", "Toi-même, %s!", "C'est celui qui le dit qui l'est, %s!", "Tu me provoques, %s?", "Diantre, %s! Je sens monter en moi la colère!", "Je te préviens, %s: la moutarde me monte au nez!", "Je sens que je m'énerve, %s.", "À genoux, %s!", "/me inspire un grand coup, puis expire lentement..."},
					  -6, {"Mortecouilles, %s, je m'en vais vous estropier! Montjoie! Saint-Denis!", "Demande pardon ou va rôtir en enfer!", "Je ne supporte pas qu'on me manque de respect!", "Dehors, odieux personnage!", "Rahh! Ne recommence plus jamais!", "Ta mère en tongs au Prisunic. :p", "Tiens, prends ça!", "Moule à gaufres!", "Du balai!", "Dégage, connard!", "Va mourir, %s!", "Pitécanthrope!", "Australopithèque", "Je veux te voir me supplier de te pardonner!", "À genoux, vermisseau!", "Nyctalope!", "Recommence pour voir!", "Implore mon pardon à genoux, je serai peut-être magnanime!", "Anacoluthe!", "Espèce de  bachibouzouk à cornes!", "C'était marrant. Recommence qu'on voie ce qui se passe.", "Ventre Saint-Gris!", "Bretzel liquide!", "Palsembleu!", "Rentre chez ta mère, embryon!"})
	  end

	  --compliment
	  if (Flags.COMPLIMENT or Flags.VIVE) and not Flags.JESUIS and not Flags.QUESTION and not Flags.INJURE and not Flags.ETTOI and not Flags.AMIGA and not Flags.WINTEL and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  2, {"Merci %s.", "C'est trop, %s.", "C'est gentil, %s.", "Tu es trop bon, %s.", "Mais, toi aussi %s.", "/me remercie vivement %s."},
				  1, {"T'es hypocrite, %s.", "%s: Espèce de faux-jeton!", "Tu le penses meme pas, %s.", "Et tu penses que je vais croire que tu le penses, %s? :)", "T'arrêtes de faire du lèche-bot, %s?", "/me pense que %s n'est pas sincère."})
	  end

	  --tu es modeste
	  if Flags.MODESTE and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  1, {"Je sais %s: je le suis le plus modeste du monde.", "Moi, %s? Modeste? J'oserais pas.", "À quoi tu vois ça, %s?", "/me ne croit pas qu'il soit modeste."},
				  2, {"Plus que toi, en tout cas, %s.", "/me est plus modeste que %s, ça c'est sûr!", "Demain j'apprends la férocité, %s. :-]"})
	  end

	  --speak english
	  if Flags.PARLER and Flags.ENGLISH and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  0, {"Sorry, %s, I don't speak English. Only French.", "Désolé, %s, je ne parle pas anglais. Seulement français.", "Sorry, %s, I am the only bot speaking French.", "/me can't speak English. Only French."},
				  0, {"I don't speak English, %s. Only french.", "Je ne parle pas anglais, %s. Seulement français.", "/me refuses to speak English."})
	  end
	  --speak french
	  if Flags.PARLER and Flags.FRENCH and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  0, {"Bien sûr que je parle français, %s.", "Of course I speak french, %s.", "Je parle un peu français.", "/me parle français."},
				  0, {"Évidemment que je parle français, %s.", "Je ne parle pas anglais, %s. Seulement français.", "/me n'a pas appris d'autre langue que le français."})
	  end

	  --mon ami
	  if Flags.TU_ES and Flags.AMI and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  2, {"Toi aussi tu es mon ami, %s.", "J'espère bien, %s.", "Super! J'aime avoir des amis.", "/me est le copain de %s, et il apprécie..."},
				  1, {"Toi tu n'es pas mon ami, %s.", "Peut-être que je t'apprécierai, quand tu seras moins ch*ant.", "/me n'aime pas %s."})
	  end

	  --ton ami
	  if Flags.JESUIS and Flags.AMI and (Flags.NOM or not isChannel) then
		 repondre(from, to,
				  2, {"Oui, %s.", "Tu es mon ami, %s.", "Tu es de mes amis, %s.", "/me est un copain de %s..."},
				  1, {"Toi tu n'es pas mon ami, %s.", "Peut-être que je t'apprécierai, quand tu seras moins ch*ant.", "/me n'aime pas %s."})
	  end


   else
	  -- on n'a PAS l'autorisation de parler =======================================================================================================
	  if (Flags.PARLER or Flags.REVEILLE_TOI) and (not Flags.LANGUE) and (not Flags.FONCTION_SAY) and ((not isChannel) or Flags.NOM) then
		 if rellevel(from) >= 0 then
			set_talk(true)
		 end

		 RespGood = {"%s: Je ne suis plus gênant? :)", "À vos ordres, %s.", "Il sera fait selon vos désirs, maître %s.", "%s: Pour me faire taire à nouveau, demandez-le moi, tout simplement.", "Avec plaisir, %s.", "Depuis le temps que j'attendais qu'on me le dise, %s! :)"}

		 if Flags.STP then
			RespBad = {"Bon! C'est bien parce que tu l'as demandé gentiment, %s!", "%s: On m'a dit de me taire, mais comme tu as été poli, j'agrée ta demande.", "Tu m'es antipathique, %s, mais je ne me tairai plus.", "Et pis j'en ai marre de me taire!", "Bon. J'ai pas envie de te faire plaisir, %s, mais j'en ai ras la casquette d'être obligé de me taire...", "Tu me fais pitié, %s. Pour l'instant je vais me remettre à parler..."}
			repondre(from, to, 1, RespGood, 2, RespBad)
		 else
			RespBad = {"Nan! Demande-le gentiment, %s!", "%s: On m'a dit de me taire, je me tairai!", "Pas question! Je n'obéis pas à ceux qui me sont antipathiques, %s!", "Et pis quoi encore, %s! Cent balles et un Mars?", "Non. J'ai pas envie de te faire plaisir, %s.", "T'as une chance si tu demandes a quelqu'un que j'aime bien, %s. Pour l'instant je continue à me taire. Na!"}
			repondre(from, to, 1, RespGood, 0, RespBad)
		 end

	  end --redonne autorisation

   end --autorisation de parler ?
   
   if Flags.GROS_MOT and (not Flags.COMPLIMENT) and (not Flags.CLINDOEIL) then
	  repondre(from, to, 
				-1, {},
				-2, {"Sois poli, s'il-te-plaît, %s!", "", "Un peu de tenue, %s!", "", "Tu vas arrêter d'être grossier, %s?"})
   end

   -- ajout de phrases aléatoires
   if (nbphrases % 500) == 100 then
	  repondre(from, to,
			   0, {"C'est peut-être hors propos, mais je vous propose une petite visite de la page de mon créateur : http://francois.parmentier.free.fr/index.html", "Vous saviez que j'ai un cousin écrit en C ? http://ector.sourceforge.net/", "Pour relancer le débat: l'Amiga c'est quand-même mieux que le PC.", "Ma famille est grande, j'ai aussi un cousin écrit en python : http://code.google.com/p/pyector/", "Hop !", "Une petite documentation sur IRC: http://www.funet.fi/~irc/.", "Atchoum!", "Si vous vous ennuyez, vous pouvez toujours jouer a pierre-papier-ciseaux avec moi.", "Si jamais ça vous tente, vous pouvez jeter un oeil sur une page de geek : http://sebastien.kirche.free.fr", "Vous saviez que Sébastien à mis sa config Emacs/Gnus en ligne sur http://sebastien.kirche.free.fr/emacs_stuff/ ? Je sais: c'est en dehors de la conversation, mais que voulez-vous, faut bien que j'fasse un peu d'pub de temps en temps...", "Perl c'est bien. Ruby c'est mieux"},
			   0, {"Encore quelqu'un qui encombre le canal!", "T'es toujours là, %s?", "%s: :PPPP", "C'est pas vrai! Tu persistes, %s!", "%s: Ta mère en short sur internet :P", "Encore là, %s? Pfff...", "%s: Pas encore parti, toi? Zut!", "Tu ne m'as toujours pas demandé pardon, %s, au fait!", "Pourquoi tu ne me demandes pas pardon, j'suis pas complètement ingrat, tu sais, %s?"})

   end
   
   if Flags.FONCTION then
	  set_talk(AncienneAutorisation)
   end

   

   
--[[
   if Flags.ILYA and Flags.QUELQUUN and Flags.QUESTION then
	  repondre(from, to, 
			   1, {"Mais oui %s: tu es là.", "Il y a au moins toi et moi, %s.", "Ouhou, %s, je suis là.", "Oui.", "Bien sûr %s, je suis là.", "Je suis là, %s, comme toujours.", "%s: Évidemment qu'il y a quelqu'un!", "/me est là."}, 
			   0, {"Qu'est-ce que ça peut te faire, %s, qu'il y ait quelqu'un ou non? De toute facon on ne souhaite pas ta présence (sauf si tu te conduis gentiment).", "Je ne sais pas si on peut considérer que comme tu es là, il y a quelqu'un, %s?", "Ça dépend, %s: te considères-tu comme une personne à part entière?"})
   end
  
    if Flags.C_EST and Flags.CALME then
	   repondre(from, to, 
				1, {"C'est le calme avant la tempête?", "Le silence t'angoisse, %s?", "Ça ne va peut-être pas durer.", "Profitons-en. Si ça se trouve, ça va pas durer.", "C'est pas de ma faute, %s!", "Ça repose, non?", "%s: C'est le moment pour faire son yoga!"},
				0, {"%s: Et j'espère que ça va continuer.", "Comme tu es là, %s, je pense que ça ne va pas durer!", "Oui, %s, c'était calme avant que tu n'arrives!", "%s: J'ai peur que ça ne dure pas!"})
	end
]]

	if string.find(from, "seki@falbala.seki.fr") then
	   if string.find(message,"phrasescount ?") == 1 then 
		  repondre(from, to, 0, {"sur " .. to .. ", " .. nbphrases .. " traitees"}, 0, {})
	   end
	   if string.find(message,"luaversion ?") == 1 then
		  repondre(from, to, 0, {"je fonctionne avec " .. _VERSION}, 0, {})
	   end
	end
	--[[
	if string.find(from, "!n=karim@") then
	   repondre(from, to, 0, {"mon copain kh", "tu me rappelles quelqu'un, kh"}, 0, {})
	end
    ]]
	
	--repondre(from, to, 0, {"fin d'analyse"}, 0, {})


end
