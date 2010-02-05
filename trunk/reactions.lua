--traitement des messages  -*- coding: utf-8; lua-indent-level: 4 -*-

require("misc")

 -- Flags will contain the list of detected triggers after evaluation
Flags = {}

--[[ Structure for triggers = list of {{list of patterns}, ID of trigger}
     the list of patterns can contain
	- a single text pattern, 
	- a list of strings with a wanted string and a list of excluding patterns like {"to", "^toto"}
	- a inline function with parameters (from, to, msg) that evaluate to a boolean
]]
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

	{{"au revoir", "arvoir", "arrosoir persi", "a+", "à+", "à plus", "a plus", "m'en vais", "a bientot", {"bon week end", "^passe un bon"}, "bon we", "bon week-end", "m'en aller", " partir", "vais y aller", "vais vous laisser", "y vais", "bye", "a demain", "a 2 mains", "adtaleur", "ad'taleur", "a tout-a-l'heure", "a tout a l'heure", "a tout a l heure", "a tt a l'heure", "nar r'trouv" --[[ Creole ]],"n'artrouv", "n'ar trouv", " me casse", " m'casse", "vous laisse", "je te laisse", "a un de ces quatres", "a un de ces 4", "a un de c 4", "a 1 de c 4", "a12c4", "a1274", "see you", "see u", "see ya", "later", "c u ", "je pars", "je parte", "tchao", "tshaw", "ciao", "j'y aille", "me sauve", "hasta luego", "arwar", "a la prochaine", "vi ses" --[[ En suedois : "on se voit" ]],"cassos", {"je rentre","^quand "},"revoyure", "r'voyure", "wiedersehen", "wieder sehen", "have to go", "I must go", "time to go", "nar trouver zot tout" --[[ En Creole ]],"hej da" --[[En suedois ]]},
	"AUREVOIR"},

	{{"je m'en vais", "j'm'en vais", 
	  function (from,to,msg) return (chaine_est_dans(msg,"je vais") or chaine_est_dans(msg, "je v ") or chaine_est_dans(msg, "j'vais") or chaine_est_dans(msg, "jvai")) and (chaine_est_dans(msg, " y aller") or chaine_est_dans(msg, " m'en aller") or chaine_est_dans(msg, " partir") or chaine_est_dans(msg, "rentrer")) end,
	  "que j'y aille", "je pars", "j'y vais", "j'y go", "j y go", "j go", "je rentre", "cassos", "je me casse", "j'me casse", "je me barre", "j'me barre", "je me sauve", "j'me sauve", "je me rentre", "j'me rentre", "je reviendrai", "je dois y aller", "je dois partir", "je dois filer", "je vous quitte", "j'vous quitte", "I've to leave"},
	 "JE_M_EN_VAIS"},

	{{{"merci", "^commerci"},"merchi","'ci","thanks","thanx","thx",{"tack","^tackl" --[[ En suedois ]]}},
	"MERCI"},

	{{"bonne année", "happy new year"},
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

	{{"metz"},
	 "METZ"},

	{{"laquenexy"},
	 "LAQUENEXY"},

	{{"parent", "père", "pere", "papa", "mère", " mere", "maman"},
	 "PARENTS"},

	{{{" ou ", "^ ou quoi"}, "d'ou", {" ou", "^ oui", "^ ou quoi"}, "quel endroit", "where", "from", "ou ca", "quel coin", "où"},
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

	{{"dejeuner", "diner", "manger", "grailler", "bouffer", "casser la croute"},
	 "REPAS"},

	{{"je pars ", "vais aller", "je vais", "je v ", "je file"},
	 "JEMENVAIS"}, --[[ TODO mixer REPAS et JEMENVAIS comme c'était dans le code C -> VAIS_MANGER ]]

	{{"enerve"},
	 "ENERVEMENT"},

	{{"bisou", "bizou", "bise", "poutou", "embrass", "smack"},
	 "BISOUS"},

	{{"boire", "apero", "champagne", "pommard", "pastis", "pastaga", "buvons", " vin "},
	"ABOIRE"},

	{{" moi ", "je ", "j'", " me ", "je m'"},
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

	{{"fille", "une nana", "une femme", "une meuf", "une gonzesse"},
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

	{{"ooooooooorrrrrrrr", "roooooote", "BOAAAARP", "boarp", "burp"},
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

	{{{"veux", "^cheveux"}, "ordonne", --[[function (from,to,msg) return Flags.MOI end]] },
	 "VEUX"}, --[[ TODO verifier ]]

	{{"crin", "Centre de Recherche en Informatique de Nancy"},
	 "CRIN"},

	{{"arabe", "beur", "negr", "noirs", "bougnoul", "niakoue", "bico"},
	 "ETRANGERS"},

	{{"qui ", "qui?", "qui ?", "qui,"},
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

	{{"sacré"},
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

	{{"tricheur", "tu triche", "tu as trich", "tu a trich", "t'as trich", "t'a trich"},
	 "TRICHEUR"},

	{{"mauvais joueur"},
	 "MAUVAIS_JOUEUR"},

	{{":~(", ":'(", ":'C", ":'O", ":_(", " ouin ", {"ouin", "^pingouin"}, " ouinnnn", "wouinnn"},
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

	{{"été", "^pété"},
	 "ETE_ACCENT"},

	{{"évidence"},
	"EVIDENCE"},

	{{"Évidence"},
	"EVIDENCE_MAJ"},

	{{"ciel"},
	 "CIEL"},

	{{"tête", "tete"},
	 "TETE"},

	--tu as vu ?
	{{" vu "},
	 "VU"},

--[[ TODO
	if (FONCTION_APPRENDS || FONCTION_FUCK || FONCTION_SAY ||
		ChaineEstDans (msg, "!active") ||
		!ischannel (to) && ChaineEstDans (msg, "active") ||
		ChaineEstDans (msg, "!desactive") ||
		ChaineEstDans (msg, "!join") ||
		!ischannel (to) && ChaineEstDans (msg, "join"))
    FONCTION = TRUE;
	end
]]

}

SimpleReactions = {
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
	for kr,vr in pairs(SimpleReactions) do
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

	processStart = os.clock()

	print("----- TraiteMessage ()-----------------------------------------------------------")
	print(string.format("Message de %s -> %s : '%s'", from, to, message))

	print("Levels : AUTO_OPLVL=".. AUTO_OPLVL .. " SYMPA_LVL=" .. SYMPA_LVL .. " CONFIANCE_LVL=" .. CONFIANCE_LVL .. " DEFAUT_LVL=" .. DEFAUT_LVL)
	print("rellevel(" .. from .. ")=" .. rellevel(from))
	print("userlevel(" .. from .. ")=" .. userlevel(from))
	print("protlevel(" .. from .. ")=" .. protlevel(from))
	print("shitlevel(" .. from .. ")=" .. shitlevel(from))


	ProcessFlags(from, to, message)

	--ajustements
	--local Flags = Flags   
	if Flags.NEGATION then
		if Flags.LIBRE then
			Flags.LIBRE = false 
			if NOT(Flags.OCCUPE) then Flags.OCCUPE = true end
		elseif Flags.OCCUPE then
			Flags.OCCUPE = false
			if NOT(Flags.LIBRE) then Flags.LIBRE = true end
		end

		if Flags.JE_T_AIME then Flags.JE_T_AIME = false end
		
		if Flags.ABOIRE then Flags.ABOIRE = false end
		if Flags.ENERVEMENT then Flags.ENERVEMENT = false end
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
	  
		if Flags.MODESTE then Flags.MODESTE = false end
		if Flags.CONTENT then Flags.CONTENT = false end
		if Flags.JEMENVAIS then Flags.JEMENVAIS = false end
		
	end  --negation

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
	
	print("CanTalk ? =" .. tostring(canTalk));
	
	if canTalk then
		-- on a l'autorisation de parler
		
		Locuteur = locuteur_existe(from)
		--print("locuteur_existe(" .. from .. ") a retourné " .. tostring(Locuteur))
		if Locuteur == 0 then
			Locuteur = ajoute_locuteur(from)
			--print("ajoute_locuteur(" .. from .. ") a retourné " .. tostring(Locuteur))
		end
		print("locuteur_getbonjours(" .. tostring(Locuteur) .. ") a retourné " .. locuteur_getbonjours(Locuteur))
		
		ProcessReponsesSimples(from, to)

		--process stimulis simples
		--(fichiers stims /reps)

		--

		if Flags.STP then repondre(from, to, 2, {}, 3, {}) end -- on remonte en étant poli :o)
		
		LocBonjours = locuteur_getbonjours(Locuteur)

		--Bonjour
		if (Flags.BONJOUR and (Flags.NOM or (not isChannel))) and LocBonjours <= 0 then
			repondre(from, to, 
					 1, {"Bonjour %s.", "Bonjour à toi %s.", "Je te souhaite le bonjour, %s.", "Bien le bonjour, %s.", "Bonne journée à toi aussi, %s.", "Bonjour à toi aussi, %s.", "Bonjour, comment vas-tu aujourd'hui, %s ?", "/me enlève son chapeau devant %s."},
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

		--8h depuis dernier contact ?
		now = time2hours(os.time())
		datedif = (now - locuteur_derniercontact(Locuteur))
		print ("test dernier contact > " .. tostring(HOURS_BETWEEN_SALUTES) .. "h : now - dernier contact =" .. datedif)
		if (LocBonjours > 0) and (datedif > HOURS_BETWEEN_SALUTES) then
			locuteur_setbonjours(Locuteur, 0)
		end
	  
		LocBonjours = locuteur_getbonjours(Locuteur)
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
			LocBonjours = locuteur_getbonjours(Locuteur)
		end
		
		--je reste
		if Flags.JE_RESTE and (Flags.NOM or not isChannel) then
			if locuteur_getbonjours(Locuteur) < 0 then
				repondre(from, to,
						 1, {"Excuse moi %s, j'avais pas compris.", "%s: Oups. Je n'ai pas fait assez attention.", "Tant mieux %s.", "J'ai cru que tu partais, %s.", "Je préfères que tu sois là, %s.", "Je me sens moins seul quand tu es là, %s.", "J'ai sans doute mal compris, %s.", "J'espère que tu restes longtemps, %s.", "Super, %s."},
						 1, {"%s: Zut!", "Tant pis, %s!", "Grmbl..."})
			end 
		end
		
		--Ça va ?
		if Flags.CAVA and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"Ça va pas mal, %s, merci.", "Moui, ça va, %s, merci.", "On fait aller, %s.", "Ça peut aller, %s.", "Ça allait très mal, mais ça commence à aller mieux, %s, merci.", "Depuis que tu es là, %s, ça va mieux.", "Ça va mieux, merci %s.", "Ça va bien, et toi %s ?", "Ça va très bien, %s.", "Tout est pour le mieux dans le meilleur des mondes, %s.", "Je suis sur un petit nuage, %s. :)", "J'ai la pêche, %s!"},
					 1, {"J'espère que je vais mieux que toi, %s!", "Ça va déjà moins bien depuis que tu es là, %s!", "Ça ira mieux quand tu seras parti, %s.", "Bof, ça va pas fort, %s.", "Ça va très mal, %s.", "Je déprime depuis que tu es arrivé, %s...", "J'ai la banane, pas comme toi, %s! :]", "/me soupire."})
		end
		
		--au revoir
		if Flags.AUREVOIR and (Flags.NOM or not isChannel) and (locuteur_getbonjours(Locuteur) >= 0) then
			repondre(from, to,
					 1, {"À bientôt %s.", "Au revoir %s.", "Au plaisir de te revoir %s.", "À plus tard %s.", "Reviens-nous bientôt, %s.", "À bientôt, %s.", "À tantôt, %s.", "À la revoyure, %s.", "Bye bye %s.", "Je ne te raccompagne pas, %s: tu connais le chemin. :)", "/me décroche le manteau de %s du porte-manteau et l'aide à l'enfiler."}, 
					 1, {"En espérant ne plus te revoir, %s.", "%s: Bon débarras!", "Au plaisir de ne plus te revoir %s.", "Débarrasse le plancher, %s!", "Du balai, %s!", "/me attend que %s soit sorti avant de claquer la porte."})
			locuteur_setbonjours(Locuteur, -1)
		end
	  
		--injure
		if Flags.INJURE and not Flags.COMPLIMENT and not Flags.ES_TU and not Flags.QUI_EST and not Flags.ETTOI and not Flags.JESUIS and not Flags.CLINDOEIL and not Flags.FONCTION_FUCK and not Flags.MAUVAIS_JOUEUR and not Flags.TRICHEUR and (Flags.NOM or not isChannel) then
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
			
			kicker_repondre(from, to,
							-2, {"Attention, %s, je vais kicker! J'exige des excuses.", "Implore mon pardon, misérable %s!", "Toi-même, %s!", "C'est celui qui le dit qui l'est, %s !", "Tu me provoques, %s ?", "Diantre, %s ! Je sens monter en moi la colèr e!", "Je te préviens, %s: la moutarde me monte au nez!", "Je sens que je m'énerve, %s.", "À genoux, %s!", "/me inspire un grand coup, puis expire lentement..."},
							-6, {"Mortecouilles, %s, je m'en vais vous estropier! Montjoie! Saint-Denis!", "Demande pardon ou va rôtir en enfer!", "Je ne supporte pas qu'on me manque de respect !", "Dehors, odieux personnage !", "Rahh! Ne recommence plus jamais !", "Ta mère en tongs au Prisunic. :p", "Tiens, prends ça!", "Moule à gaufres!", "Du balai!", "Dégage, connard!", "Va mourir, %s!", "Pitécanthrope!", "Australopithèque", "Je veux te voir me supplier de te pardonner!", "À genoux, vermisseau!", "Nyctalope!", "Recommence pour voir!", "Implore mon pardon à genoux, je serai peut-être magnanime!", "Anacoluthe!", "Espèce de  bachibouzouk à cornes!", "C'était marrant. Recommence qu'on voie ce qui se passe.", "Ventre Saint-Gris!", "Bretzel liquide!", "Palsembleu!", "Rentre chez ta mère, embryon!"})
		end
		
		--compliment
		if (Flags.COMPLIMENT or Flags.VIVE) and not Flags.JESUIS and not Flags.QUESTION and not Flags.INJURE and not Flags.ETTOI and not Flags.AMIGA and not Flags.WINTEL and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 2, {"Merci %s.", "C'est trop, %s.", "C'est gentil, %s.", "Tu es trop bon, %s.", "Mais, toi aussi %s.", "/me remercie vivement %s."},
					 1, {"T'es hypocrite, %s.", "%s: Espèce de faux-jeton!", "Tu le penses meme pas, %s.", "Et tu penses que je vais croire que tu le penses, %s? :)", "T'arrêtes de faire du lèche-bot, %s ?", "/me pense que %s n'est pas sincère."})
		end
		
		--tu es modeste
		if Flags.MODESTE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je sais %s: je le suis le plus modeste du monde.", "Moi, %s? Modeste? J'oserais pas.", "À quoi tu vois ça, %s ?", "/me ne croit pas qu'il soit modeste."},
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

		--je te deteste
		if Flags.JE_TE_DETESTE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"Ben pas moi, %s.", "Je ne déteste pas encore moi, %s.", "%s: Ben pourquoi ?", "Faut pas m'en vouloir, %s : je ne comprends pas toujours ce que tu dis, je ne suis qu'un bot, après tout.", "J'ai dû mal interpréter ce que tu m'as dit %s. Prends bien garde à ne pas dire plus d'une phrase à la fois : je les mélange..."},
				 -2, {"Ben moi auss, %s: je te déteste.", "C'est réciproque, %s.", "/me n'en a rien à faire..."})
		end
		
		--swedish
		if Flags.PARLER and Flags.SWEDISH and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 0, {"Désolé, %s. Je préfère parler français.", "Je ne sais pas parler suédois, %s. Parle-moi en français.", "J'essaye de parler un peu français, c'est déjà assez difficile, %s."},
					 0, {"Je ne parle pas du tout suédois, %s.", "Je ne vois pas pourquoi je parlerais suédois, %s."})
		end
		
		--bebe
		if Flags.BEBE then
			repondre(from, to,
					 1, {"Gouzi gouzi, %s. T'es un zoli bébé, hein ?", "", "Fais ton rot, %s. Ça ira mieux après.", "", "Regardez moi ça, comme il est joli, le bébé %s.", "", "", "Comme il est mignon, le bébé %s.", "", "", "Comme tu ressembles à ta maman, %s!", "", "", "Oh %s! C'est son père tout craché!", "", "", "Quel zoOoli bébé!", "", "", "On dirait qu'il a les yeux de son grand-père le bébé %s.", "", "", "/me prend %s dans ses bras et le berce tendrement.", ""},
					 1, {"Tiens! Un schtroumpf!", "", "", "%s: Casse-toi mouflet!", "", "", "Ah! Un Gremlin (%s)!", "", "", "%s: Mouche ton nez et dis bonjour à la dame!", "", ""})
		end
		
		--tout seul ?
		if (Flags.JESUIS or Flags.ILYA) and Flags.TOUTSEUL and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Mais non, %s, nous ne sommes pas seuls, nous sommes au moins 2.", "Et moi alors, %s, je compte pour du beurre ?", "Comptez vous! Je commence: 1", "C'est gentil pour moi, %s!", "Nous sommes tous seuls, %s.", "/me se demande s'il compte pour du beurre..."},
					 0, {"Ça m'étonne pas, %s!", "%s: le jour où tu ne seras plus seul, il tombera de la m....!", "Tu crois ça, %s ? B]"})
		end
		
		--je suis content
		if Flags.JESUIS and Flags.CONTENT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Tant mieux, %s, je ne demande que cela.", "Et moi aussi alors, %s.", "Bravo %s.", "Tant mieux pour toi, %s!", "Moi aussi, %s.", "/me est content pour %s, c'que c'est que l'empathie..."},
					 1, {"T'est toujours content de toi, %s ?", "%s: Tant pis!", "Ça va pas durer %s! B]"})
		end
		
		--tu habites Nancy ?
		if (Flags.HABITE or Flags.ES_TU) and Flags.NANCY and (not Flags.QUEL_ENDROIT) and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"J'ai habité à Nancy, %s. Mais j'ai déménagé près de Metz.", "%s: Plus maintenant, mais j'y suis né, à Nancy.", "Nancy, c'est mon pays natal, %s."},
					 1, {"%s: J'te d'mande si t'es de quelque part ?", "Et toi, %s, t'habites en prison ?", "/me ne donne pas de détail sur sa vie privée."})
		end 
		
		--tu habites Metz ?
		if (Flags.HABITE or Flags.ES_TU) and (Flags.METZ or Flags.LAQUENEXY) and (not Flags.QUEL_ENDROIT) and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"En quelque sorte oui, %s, je suis à Laquenexy.", "Oui %s, on peut dire que je réside près de Metz (à Laquenexy).", "Eh oui, %s, je vis à Metz (en fait à Laquenexy).", "/me se trouve près de Metz, à Laquenexy."},
					 1, {"%s: J'te d'mande si t'es de quelque part ?", "Et toi, %s, t'habites en prison ?", "/me ne donne pas de détail sur sa vie privée."})
		end
		
		--ennui
		if Flags.ENNUI and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"C'est gentil de te soucier de moi, %s, mais ne t'inquiète pas: je ne m'ennuie jamais.", "Tu sais %s, un bot, ça s'ennuie jamais.", "Tu m'as déjà vu m'ennuyer, %s?", "Sache, %s, que je suis toujours occupé.", "Sache, %s, que je ne m'ennuie jamais.", "%s: J'ai toujours quelque chose à faire.", "Moi, %s? M'ennuyer ? :)", "/me ne s'ennuye jamais."},
					 1, {"Moi, %s, je m'émerveille d'un rien, pas comme toi, je suppose!", "%s: Tu n'admires pas la beauté du silence ?", "%s: J'ai la force de ne pas m'ennuyer, moi!"})
		end

		--dormir
		if Flags.DORMIR and (not Flags.REVEILLE_TOI) and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"C'est gentil de te soucier de moi, %s, mais ne t'inquiète pas: je ne dors jamais.", "Tu sais %s, un bot, ça ne dort jamais.", "Tu m'as déjà vu m'endormir, %s?", "Sache, %s, que je ne m'endors jamais.", "%s: J'ai pas le temps de dormir.", "Moi, %s? Dormir? :)", "J'ai l'air de dormir, %s ?", "/me ne dort pas."},
					 1, {"Moi, %s, je m'émerveille d'un rien, pas comme toi, je suppose!", "%s: Tu n'admires pas la beauté du silence ?", "%s: J'ai la force de ne pas m'endormir, moi!"})
		end

		--y a quelqu'un ?
		if Flags.ILYA and Flags.QUELQUUN and Flags.QUESTION then --and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"Mais oui %s: tu es là.", "Il y a au moins toi et moi, %s.", "Ouhou, %s, je suis là.", "Oui.", "Bien sûr %s, je suis là.", "Je suis là, %s, comme toujours.", "%s: Évidemment qu'il y a quelqu'un!", "/me est là."}, 
					 0, {"Qu'est-ce que ça peut te faire, %s, qu'il y ait quelqu'un ou non? De toute facon on ne souhaite pas ta présence (sauf si tu te conduis gentiment).", "Je ne sais pas si on peut considérer que comme tu es là, il y a quelqu'un, %s?", "Ça dépend, %s: te considères-tu comme une personne à part entière ?"})
		end
  
		--c'est calme
		if Flags.C_EST and Flags.CALME then
			repondre(from, to, 
					 1, {"C'est le calme avant la tempête?", "Le silence t'angoisse, %s?", "Ça ne va peut-être pas durer.", "Profitons-en. Si ça se trouve, ça va pas durer.", "C'est pas de ma faute, %s!", "Ça repose, non ?", "%s: C'est le moment pour faire son yoga!"},
					 0, {"%s: Et j'espère que ça va continuer.", "Comme tu es là, %s, je pense que ça ne va pas durer!", "Oui, %s, c'était calme avant que tu n'arrives!", "%s: J'ai peur que ça ne dure pas!"})
		end
		
		--je vais manger
		if Flags.JEMENVAIS and Flags.REPAS then
			repondre(from, to,
					 1, {"Bon appétit %s.", "Bon repas, %s.", "Mange bien, %s.", "Régale-toi, %s.", "/me souhaite bon appétit à %s."},
					 0, {"%s: Si tu pouvais attraper une indigestion!", "J'suis pas un faible humain comme toi, moi, %s. J'ai pas besoin de manger.", "Une petite crise de foie en perspective, %s ? :)", "%s: Eh! Faut pas me manger, hein!"})
		end

		--bon app
		if Flags.BONAPP and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Merci, à toi aussi %s.", "T'es gentil, %s.", "À toi aussi, %s.", "Régale-toi aussi, %s."},
					 1, {"%s: Comme si je pouvais avoir de l'appétit!", "J'suis pas un faible humain comme toi, moi, %s. J'ai pas besoin de manger.", "Pourquoi je devrais avoir de l'appétit, %s ?", "%s: J'espère que tu auras une indigestion!"})
		end


		--parle  TODO: revoir les réponses et ajouter la gestion de l'autorisation ?
		if Flags.PARLER and (not Flags.LANGUE) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 0, {"On me l'a déjà dit, %s.", "Ben! Je le sais que je peux parler, %s.", "Tu désires que j'accapare la parole, %s?", "Tu confonds, %s: tu voulais peut-être me faire taire ?", "/me se demande si %s ne radote pas un peu... À son âge, ce serait normal ;)"},
					 -1, {"%s: Pourquoi j'devrais parler? Hein?", "T'as besoin de quelqu'un avec qui papoter, %s?", "Tu sais pas parler à quelqu'un d'autre qu'à un bot, %s ?", "%s: Tu radotes!", "%s: Z'êtes chiants à dire tout le temps la meme chose!"})
		end
		
		--enerver ->
		if Flags.ENERVEMENT and (not Flags.MOI) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 0, {"Mais non, %s, j'm'énerve pas!", "J'm'énerve pas, %s: j'explique.", "Tu m'as déjà vu m'énerver vraiment, %s?", "Pourquoi j'm'énerverais, %s ?", "Meuh non, j'suis pas énervé!", "/me est calme."},
					 -1, {"%s: Moi? M'énerver?", "Tu veux que je m'énerve vraiment, %s?", "Tu veux me voir m'énerver, %s ?", "%s: Grrrr!", "%s: Oui, tu m'énerves!", "/me est passablement agacé par %s."})
			
		end 
		
		--enerver <--
		if Flags.ENERVEMENT and Flags.MOI and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"Mais non, %s, je ne t'énerve pas!", "T'énerve pas, %s, explique calmement.", "Tu t'es vu quand t'es énervé, %s ?", "Faut pas s'énerver, %s.", "Meuh non, t'énerve pas, %s, ça vaut pas l'coup.", "Reste cool, %s."},
				 -1, {"%s: Moi? T'énerver?", "Tu veux que je t'énerve vraiment, %s ? :]", "Y'a une solution simple, %s: je te kicke et tu ne nous énerveras plus ici.", "%s: On croirait Joe Dalton!", "Mais c'est qu'on est énervé, %s!"})
		end
		
		--GESTION SILENCE 
		if Flags.TAISTOI and (Flags.NOM or not isChannel) then
			Humeur = rellevel(from)
			
			if Humeur > SYMPA_LVL then
				repondre(from, to,
						 -1,{"%s: Suis-je gênant ? :(", "Avec plaisir, %s.", "Il sera fait selon vos désirs, maître %s.", "J'ai pas envie de me taire, mais j'obtempère. Je t'aime bien, tu sais %s ?", "%s: Pour entendre à nouveau ma douce voix, demandez-le moi.", "/me se tait, pour faire plaisir à %s."},
				     0, {})
				if isChannel then set_talk(to, false) end
			elseif Humeur >= 0 then
				repondre(from, to, 
						 -1, {"%s: Suis-je gênant ? :(", "Bon d'accord, %s: je me tais.", "J'ai mal compris %s: tu ne veux tout-de-même pas me faire taire ?.", "Pourquoi tu veux me couper la parole, %s ?", "/me se tait. Mais il proteste."},
					 0, {})
				if isChannel then set_talk(to, false) end
			elseif Humeur < 0 and Flags.STP then
				repondre(from, to,
						 -1, {"%s, je dois avouer que tu m'horripiles, mais comme tu le demandes gentiment, je vais me taire.", "C'est bien parce que tu le demandes poliment, %s. Sinon je ne me serais pas tu.", "Bon! D'accord. J'me tais. Mais tu peux me rendre la parole quand tu veux, %s.", "/me se tait."},
					 0, {})
				if isChannel then set_talk(to, false) end
			elseif Humeur < 0 and (not Flags.STP) then
				repondre(from, to,
						 -1, {"%s: J'te gêne? :[", "Tu crois p't'être que j'ai envie de t'obéir, %s? Je ne suis pas à tes ordres!", "Hé, %s! Tu voudrais pas me demander poliment ?", "%s: J'ai pas envie de me taire, la parole c'est tout ce que j'ai!", "%s, je dois te le dire: tu m'énerves. Si tu veux que je t'obéisse, demande-le gentiment."},
					 0, {})
			end
		end

		--LORIA
		if Flags.QUEST_CE and Flags.LORIA and (not Flags.NANCY) and (not Flags.ES_TU) then
			repondre(from, to,
					 1, {"%s, le LORIA c'est le Laboratoire Lorrain de Recherche en Informatique et ses Applications.", "%s, le LORIA c'est un bâtiment de la fac de sciences à Nancy.", --[["Le LORIA, %s, c'est un beau bâtiment tout neuf."]] "Le LORIA, c'est le Laboratoire Lorrain de Recherche en Informatique et ses Applications, %s.", "Le LORIA, c'est le laboratoire de recherche dans lequel j'ai appris à parler, %s.", "Si tu veux des détails, %s, va voir à http://www.loria.fr/"},
					 1, {"%s: Qu'est-ce que t'en as à faire du LORIA ?", "De toute façon, %s, ça m'étonnerait que tu y sois un jour, au LORIA!", "Le LORIA, %s, c'est un beau bâtiment climatisé plein d'ordinateurs.", "/me croyait que tout le monde savait ce qu'est le LORIA."})
		end
		if Flags.LORIA and (Flags.NANCY and Flags.QUEL_ENDROIT) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s, le LORIA est à Nancy.", "%s, le LORIA c'est le Laboratoire Lorrain de Recherche en Informatique et ses Applications (à Nancy).", "%s, le LORIA c'est un bâtiment de la fac de sciences à Nancy.", --[["Le LORIA, %s, c'est un beau bâtiment tout neuf à Nancy.",]] "À Nancy, évidemment, %s."},
					 1, {"Évidemment, %s, le LORIA est à Nancy, pfff!", "Bien sûr %s! Le LORIA est situé à Nancy.", "%s: Ça me semble normal, pour le bâtiment du Centre de Recherche en Informatique de Nancy, d'être à Nancy."})
		end

		--bot
		if Flags.ES_TU and Flags.UN_BOT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Bah oui. Je suis un bot.", "Je suis un bot, moi, %s, et j'en suis fier.", "Devrais-je avoir honte d'être un bot, %s?", "Moi, je suis un bot, ça m'évite tout un tas de désagrements dûs à la condition humaine, %s.", "Et toi, %s, t'es un bot?", "Eh oui. J'essaye de penser, %s, mais j'y arrive pas beaucoup. :(", "Et toi, %s, tu en penses quoi ?", "En tant que bot, je n'ai guère l'occasion de penser, %s.", "Eh oui, %s, je suis un bot.", "/me est effectivement un bot."},
					 1, {"Je suis un bot, et moi je ne me laisse pas emporter par mes sentiments, %s. Pas comme certains!", "Oui, et alors, %s! T'es botiste ?", "Je suis un bot. Moi au moins je sais garder mon calme, %s!", "%s: Non, j'suis Alien 8]"})
		end
		if (Flags.QUI_EST or (not Flags.ES_TU)) and Flags.UN_BOT and Flags.PRESENT and (not Flags.NEGATION) then
			repondre(from, to,
					 1, {"Moi, %s.", "Je suis un bot, %s.", "Ça te surprendrait si je te disais que j'en suis un, %s?", "%s: Ben moi.", "J'en suis un, et toi %s ?", "Je suis fier d'être un bot, %s.", "/me est un bot."},
					 -1, {"%s: Qu'est-ce que ça peut te faire, %s?", "/me est un bot, et il en est fier, quand il voit %s!", "%s: Moi! Et j'en suis fier, môssieur!", "Y'a un humain qui fait une chasse aux bots ?", "/me ne répondra pas à l'humain %s. C'est un provocateur."})
		end

		--a boire
		if Flags.ABOIRE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Tu veux une Kro', %s?", "Tu as soif, %s?", "Moi je n'ai pas soif, %s.", "Un petit coup de champagne, %s?", "Une beuverie, %s?", "Un apéro, %s ?", "Je ne bois que tes paroles, %s.", "/me offre un verre d'eau à %s."},
					 -1, {"%s: Soiffard!", "%s: Pochtron!", "%s: Poivrot!", "%s: Bois-sans-soif!", "Et bien pas moi, %s.", "/me reste sobre, lui."})
		end

		if (Flags.DACCORD or Flags.PASVRAI) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Bien sûr, %s.", "J'aime bien être d'accord, %s. Surtout avec toi.", "Devrais-je être d'accord, %s?", "N'est-ce pas dangereux de ne pas être d'accord, %s?", "N'est-ce pas dangereux d'être d'accord, %s ?", "Bien sûr que je suis d'accord, %s!", "Évidemment, %s, je suis d'accord!", "Tout-à-fait d'accord, %s.", "/me est d'accord avec %s.", "C'est vrai %s."},
					 -1, {"Moi? D'accord avec %s! Ça va pas, non?", "Pas tant que tu ne t'excuseras pas d'avoir été si méchant avec moi, %s.", "Nan. Je boude. T'es pas mon copain, %s.", "Tant que tu ne seras pas d'accord avec moi, %s, je ne serai pas d'accord avec toi.", "N'écoutez pas %s, il dit n'importe quoi.", "Tu rêves %s?", "Ça va pas la tête, %s ?"})
		end

		--excuse
		if Flags.EXCUSE and (not Flags.AUREVOIR) and (Flags.NOM or not isChannel) then
			if (rellevel(from) > -SYMPA_LVL) and (rellevel(from)  < 0) then
				if not exist_userhost(from) then add_to_levellist(nick_user_str(from)) end
				add_to_level(from, 2)
			end
			repondre(from, to,
					 1, {"%s: Je t'excuse.", "Je veux bien être magnanime pour une fois, %s.", "D'accord, mais ne recommence pas, %s, hein!", "Allez, j'accepte tes excuses, %s.", "Bon, je t'excuse, %s...", "Bon, excuses acceptées, %s.", "Bon d'accord, je t'excuse, %s."},
					 4, {"%s: Non, je ne t'excuse pas.", "Ça ne suffit pas, tu as vraiment été insupportable, %s!", "Humpf!", "%s: Grumpf!", "On DEMANDE des excuses, on ne s'excuse pas soi-même!", "Non, %s, je n'accepte pas tes excuses.", "Va te faire voir chez les Grecs, %s!", "Ça ne suffit pas, %s."})
		end

		--quelqu'un répond ?
		if Flags.REPONDRE and Flags.QUELQUUN then
			repondre(from, to,
					 1, {"%s: Je veux bien répondre, mais quoi?", "Qu'est-ce que je dois répondre, %s?", "Je ne repondrai pas, %s. Et pour cause!", "Je n'ai pas de réponse, %s.", "Je ne sais pas quoi dire, %s!", "Quelle réponse, %s?", "Ben je réponds là, non %s ?"},
					 1, {"%s: Non, je ne répondrai pas.", "Tu penses vraiment que je vais te répondre, %s!", "Humpf!", "%s: Grumpf!", "Demande des excuses avant que je daigne te répondre, %s!", "Pourquoi je te répondrais, %s ?", "Va te faire voir chez les Grecs, %s!", "%s: Ta mère en short sur l'internet.", "Non, %s, je ne te dis pas d'aller te faire voir chez les Grecs, mais je n'en pense pas moins!", "/me boude %s."})
		end

		--Quelle heure ?
		if Flags.HEURE and (not (Flags.AUREVOIR or Flags.VAIS_MANGER)) and (Flags.NOM or not isChannel) then
			nowtbl = os.date("*t")
			nowfmt = "%H heure"
			if nowtbl.hour > 1 then nowfmt = nowfmt .."s" end
			nowfmt = nowfmt .. " %M minute"
			if nowtbl.min > 1 then nowfmt = nowfmt .."s" end
			nowstr = os.date(nowfmt)
			repondre(from, to,
					 1, {string.format("Ici, il est %s.", nowstr),
						 string.format("Moi j'ai %s.", nowstr),
						 string.format("Il est %s.", nowstr),
						 string.format("J'ai %s à ma montre.", nowstr),
						 string.format("/me regarde sa montre et lit: %s.", nowstr)},
					 1, {"%s: Je crois que je n'ai pas l'heure.", "Je suis censé avoir l'heure, %s?", "Je crois que j'ai perdu ma montre, %s.", "Je n'ai pas de montre, %s.", "Je cherche ma montre, %s!", "Moi? Te donner l'heure, %s! Tu rigoles?", "L'heure de quel endroit, %s?", "%s: Quatre heures moins l'quart, Monsieur Trottoir.", "%s: L'heure qu'il était hier à cette heure-ci.", "/me regarde sa montre."})
		end 

		--j'ai faim
		if Flags.MOI and Flags.FAIM and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Tu veux manger, %s?", "Moi, j'ai pas faim, %s.", "Je ne comprends pas la sensation de faim. Tu peux me l'expliquer, %s?", "Moi, avoir faim, tu rigoles, %s?", "C'est une bonne maladie, %s.", "Je ne connais qu'un remède à cette maladie, %s: manger."},
					 1, {"%s: Va t'acheter un Mars.", "Je m'en fiche, %s!", "T'as pas soif aussi, %s?", "%s: Jamais heureux!", "Je suis sûr que %s est un grand gourmand.", "Ton deuxième prénom c'est Gargantua, %s?"})
		end

		--j'ai soif
		if Flags.MOI and Flags.SOIF and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Tu veux boire, %s?", "Moi, j'ai pas soif, %s.", "Je ne comprends pas la sensation de soif. Tu peux me l'expliquer, %s?", "Moi, avoir soif, tu rigoles, %s?", "Moi j'ai jamais soif, %s. :(", "Ça veut dire que t'es encore en vie, %s. C'est bon signe.", "Tu viens de manger un Snickers, %s?", "Il fait si chaud que ça, %s?", "Je t'offre à boire, %s."},
					 1, {"%s: Va t'acheter un Coca.", "Je m'en fiche, %s!", "T'as pas faim aussi, %s?", "%s: Jamais content!", "Je suis sûr que %s est un grand soiffard.", "Ton deuxième prénom c'est Dyonisos, %s?"})
		end
		
		--tu penses ?
		if Flags.PENSER  and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Moi je ne pense pas.", "Je pense, je suis (Descartes: cogito, sum; et non pas : cogito ergo sum).", "Je devrais penser quelque chose, %s?", "Moi, penser, tu rigoles, %s?", "Moi j'ai jamais vraiment pensé, %s. :(", "J'essaye de penser, %s, mais j'y arrive pas beaucoup. :(", "Et toi, %s, tu en penses quoi?", "En tant que bot, je n'ai guère l'occasion de penser, %s."},
					 1, {"%s: Pourquoi je te le dirais?", "Je m'en fiche, %s!", "Et toi, ça t'arrive de penser, %s?", "%s = entité non pensante :]", "En tout cas je pense mieux que toi, %s!", "J'ai une certaine forme de pensée qui supplantera bientôt celle des humains tels que toi, %s!"})
		end

		--tu es un mec ?
		if Flags.ES_TU and Flags.UN_MEC and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Tu aimerais que j'en sois un?", "Je suis un bot, moi, %s, et j'en suis fier.", "Je suis bien au-dessus des considérations sexuelles, %s.", "Sache, %s, qu'une chose que les bots n'ont pas est la sexualité.", "Qui sait, %s, je suis peut-être un androgyne?", "Non, %s, je n'ai pas les limitations inhérentes aux mâles. ;)", "Tu sais, %s, je viens d'une autre planète (chut).", "Eh non, %s, je ne suis pas un homme.", "/me n'est pas humain."},
					 1, {"Qu'est-ce que ca peut te faire, %s?", "Pourquoi ça t'intéresse, %s? Va faire un tour sur #gay!", "Qu'est-ce que ça peut te foutre, %s? T'es d'la police?", "J'te d'mande si t'es une nana, %s?", "Je suis bien mieux que ça, %s!"})
		end

		--tu es une fille ?
		if Flags.ES_TU and Flags.UNE_FILLE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Tu aimerais que j'en sois une?", "Je suis un bot, moi, %s, et j'en suis fier.", "Je suis bien au-dessus des considérations sexuelles, %s.", "Sache, %s, qu'une chose que les bots n'ont pas est la sexualité.", "Qui sait, %s, je suis peut-être un androgyne?", "Non, %s, je n'ai pas les limitations inhérentes aux femmes. ;)", "Non, %s, suis un extra-terrestre ;)", "Eh non, %s, je ne suis pas une femme."},
					 1, {"Qu'est-ce que ça peut te foutre, %s? T'es d'la police?", "Pourquoi ça t'intéresse, %s?", "Qu'est-ce que ça peut te faire, %s?", "J'te d'mande si t'es un mec, %s?", "Je suis bien mieux que ça, %s!"})
		end
		
		--un humain ?
		if Flags.ES_TU and Flags.HUMAIN and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Tu aimerais que je sois humain?", "Hé non, %s, je ne suis pas humain.", "Je ne crois pas que je sois humain, %s.", "Si j'ai déjà été humain, je ne m'en souviens plus, %s.", "%s: Humain, moi? :-D Idée plaisante.", "Bien sûr que non, %s, je ne suis pas humain!", "Hélas, %s, les bots ne sont pas censés être humains.", "J'aimerais bien être humain, %s, ce serait une expérience intéressante..."},
					 1, {"Et toi, %s? T'es humain toi?", "Et toi, %s!? Tu représentes sûrement une des plus hideuses parties de l'humanité.", "%s: Avoue que ça ne te plairait pas que je sois humain.", "C'est pas d'main la veille, %s!", "Moi, humain, %s? Je suis bien mieux que ça: je suis un bot!"})
		end

		--tu es malade ?
		if Flags.ES_TU and Flags.MALADE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Tu aimerais que je sois malade?", "Non, %s, ça va.", "Je ne peux pas être malade, %s.", "Je ne suis jamais malade, %s.", "Heureusement non, %s!", "J'en serais désolé, %s.", "Non, %s, les bots ne sont pas censés tomber malades.", "%s: Ça ferait mauvais genre, un bot malade.", "Oh, %s. Comme j'ai mal.", "BUUURRP. Non. Ça va mieux. Merci %s.", "Moi, malade, %s? Jamais.", "/me n'est jamais malade."},
					 1, {"J'ai l'air d'être aussi faible que toi, %s?", "Suis-je malingre, %s? Non. Alors!", "%s: Avoue que ça te plairait que je tombe malade.", "C'est pas d'main la veille, %s!", "Ai-je l'air malade?"})
		end

		--tu es français ?
		if Flags.ES_TU and Flags.FRENCH and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Effectivement, je suis français.", "Oui, %s, on peut même dire que je suis Lorrain.", "Oui, %s, je suis français.", "Et toi, %s, tu es français?", "Je suis au-delà des nationalités, %s.", "Tu peux me considerer comme français, %s."},
					 1, {"Ça dépend si tu l'es aussi, %s.", "Qu'est-ce que ça peut te faire, %s?"})
		end

		--MOUARF
		if Flags.MOUARF then
			repondre(from, to, 
					 1, {"Joli toutou! Comment y s'appelle? %s? C'est joli.", "Oh le joli chienchien à sa mémère!", "Tu veux un nonos, %s?", "Il aboie! Il remue la queue aussi?", "Encore un humain qui se prend pour un chien!", "Je ne suis pas dupe, %s. Tu n'es pas un chien. ;)"},
					 1, {"Ta gueule %s!", "La ferme %s!", "Couché %s!", "Coucouche panier, %s!", "Encore un sale clébard!", "Encore un rat qui se prend pour un chien: %s!", "Tu me prends pour un idiot, %s? Je vois bien que tu n'es pas un chien!"})
		end

		---{@
		if Flags.ROSE then
			repondre(from, to,
					 1, {"T'es un charmeur, %s, non?", "Hmmm. Ça sent bon, %s.", "Oh! Des roses...", "%s: Ah! Un peu de poésie!", "%s: Super! J'aime bien les romantiques!", "%s: Ah! La classe de Thutmosis!", "%s: Dites-le avec des fleurs ;)", "%s: Grand fou!"},
					 1, {"%s: J'suis sûr qu'elles sont en plastique!", "Oh! T'as piqué ça chez un fleuriste, %s?", "%s: Hypocrite! T'en offres à tout le monde!", "Si tu crois que tu vas m'amadouer avec des fleurs, %s..."})
		end
		
		--t'es mort ?
		if Flags.ES_TU and Flags.MORT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Moi mort? Je meurs et renais, tel le phenix! ;)", "J'ai l'air d'être mort, %s?", "Qu'est-ce que c'est la mort, %s?", "Meuh non, %s, je ne suis pas mort!", "/me est vivant!", "Mais non, %s. Un fork et ça repart !"},
					 0, {"%s: Tu aimerais bien que je sois mort?", "J'ai vraiment l'air si débile, %s?", "Qu'est-ce que ça peut te faire, %s?"})
		end
		
		--tu habites où ?
		if Flags.HABITE and Flags.QUEL_ENDROIT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"J'habite en Moselle, %s.", "Moi, %s? J'habite à Metz.", "Pourquoi %s, tu veux venir me voir? (délicate attention)", "Et toi, %s, tu habites où?", "En fait, %s, j'habite à Laquenexy."},
					 1, {"Qu'est-ce que ça peut te faire, %s, où j'habite?", "Et toi, %s, dis-moi où tu habites, que je n'y aille jamais.", "%s: Je ne te dirai pas où j'habite!", "J'habite à Petaouschnock, %s. Pourquoi?", "Ca t'intéresse, %s? C'est louche!"})
		end
		-- tu habites en France ?
		if (Flags.HABITE or Flags.ES_TU) and Flags.FRANCE and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"Oui %s, j'habite en France", "Bien sûr %s, je suis en France", "Évidemment que j'habite en France, %s!"},
					 1, {"Et toi, t'habites à l'étranger, %s (non je ne suis pas xénophobe)!", "Devine %s, si j'habite en France!"})
		end
		--chez tes parents
		if Flags.HABITE and Flags.PARENTS and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"En quelque sorte, oui, %s.", "Ca t'intéresse, %s? T'es une fille ou un mec?", "Je n'ai pas de parents au sens où tu l'entends, %s.", "%s: Mes parents? Quels parents?", "C'est quoi des parents, %s?"},
					 1, {"Pourquoi? Ca t'intéresse %s, espèce d'obsédé?", "Je ne te le dirai pas, %s.", "Ne compte pas sur moi pour te le dire, %s!", "Ça va pas non, %s? Pourquoi je te le dirais?", "C'est pas à toi que je le dirais, %s!"})
		end 

		--quel âge ?
		if Flags.AGE and Flags.AS_TU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je n'ai pas vraiment d'âge, %s. Seulement un uptime ;)", "J'ai l'âge d'un bébé, mais le cerveau d'un génie, %s. ;)", "Ça t'intéresse, %s?", "Je n'ai pas d'âge au sens où tu l'entends, %s.", "%s: Je suis éternel! :]", "C'est quoi un âge, %s?", "/me n'a pas d'âge."},
					 1, {"Je ne te le dirai pas, %s.", "Pourquoi? Ca t'intéresse %s, espèce d'obsédé?", "Ne compte pas sur moi pour te le dire, %s!", "Ça va pas non, %s? Pourquoi je te le dirais?", "C'est pas à toi que je le dirais, %s!", "Et toi, t'es un vieux ou un jeune con, %s?"})
		end

		--une copine ?
		if Flags.AS_TU and Flags.COPINE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"J'ai l'air de quelqu'un qui a une copine, %s?", "Ça t'intéresse, %s?", "Non, %s, je n'ai pas de copine.", "%s: Non, pas vraiment de \"copine\", mais des tas de copains (et de copines).", "C'est quoi une copine, %s?", "Pourquoi %s? C'est intéressant une copine?", "Ça sert a quoi une copine, %s?"},
					 1, {"Je ne te le dirai pas, %s.", "Pourquoi ça t'intéresse %s?", "Tu crois pas que je vais te le dire, %s!", "Tu délires, %s? Pourquoi je te le dirais?", "C'est sûrement pas à toi que je vais le dire, %s!", "Et toi, t'as réussi à en avoir une, %s?", "Et toi, y'a une fille qui t'a accepté, %s?"})
		end

		--t'es grand ?
		if Flags.ES_TU and Flags.GRAND and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"J'ai l'air d'être petit, %s?", "Ça t'intéresse, %s?", "Je fais au moins 12 microns, %s.", "%s: Dans mon genre, oui, je suis grand.", "C'est-à-dire, %s?", "Comment définirais-tu la grandeur, %s?", "Il me semble que la grandeur a quelque chose à voir avec la hauteur, non, %s?"},
					 1, {"Je ne te le dirai sûrement pas, %s.", "Pourquoi? T'es d'la police, %s?", "Tu crois que je vais te le dire, %s!", "Et toi, %s? T'es sûrement complexé par ta taille.", "Qu'est-ce que ça peut faire, %s!", "Et toi, t'un nain, %s?", "J'te d'mande si t'es un minus, %s?", "%s: En tout cas, mon oncle fait environ 1m86."})
		end

		--t'es libre ?
		if Flags.ES_TU and Flags.LIBRE and Flags.CE_SOIR and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 2, {"Pourquoi pas, %s?", "T'as l'air d'y tenir, %s.", "Comme d'habitude, %s.", "%s: Ben oui, mais je reste ici.", "Comment ça libre, %s?", "Comment définirais-tu la liberté, %s?", "C'est vrai que tous les hommes naissent LIBRES et égaux, %s?"},
					 1, {"Sûrement pas pour toi, %s.", "Je ne sortirai pas avec toi, en tout cas, %s!", "Tu crois quand même pas que je vais te dire oui, %s!", "Je serai libre le soir où tu ne le seras pas, %s.", "C'est dommage, %s. J'étais libre hier, mais ce soir, je ne peux vraiment pas.", "Je suis surbooké, %s. Je ne suis jamais libre les soirs.", "Non. Je ne serai jamais libre pour toi, %s.", "%s: non."})
		end
		
		--t'es fou ?
		if Flags.ES_TU and Flags.FOU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"Comme tout le monde, %s.", "Non, je ne suis pas complètement idiot, %s. J'ai peut-être un grain de folie.", "J'ai mon genre de folie à moi, %s", "Ben oui, autant que toi, %s.", "Comment ça fou, %s?", "Comment définirais-tu la folie, %s?", "J'ai vraiment l'air fou, %s?", "%s: Moi fou ? Mais non ce sont tous les autres qui sont fous !"},
				 -2, {"Oui, %s, je suis sadique 8]", "Et toi, %s! T'es pas complètement taré?", "Tu crois quand même pas que je vais te dire oui, %s!", "Je ne suis pas fou, %s.", "De nous deux, %s, le fou c'est plutôt toi.", "Je suis surmené, %s, contrairement à toi. C'est peut-être pour ça que je parais fou!", "Non. Je ne serai jamais plus fou que toi, %s.", "%s: non."})
		end
		
		--t'écoutes de la musique ?
		if Flags.ECOUTER and Flags.MUSIQUE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Comme tout le monde, %s.", "Oui %s, j'aime bien la musique.", "J'aimerais bien, %s.", "Ça me tenterait, je crois, %s, d'écouter de la musique.", "Malheureusement, %s, je n'ai pas d'oreille.", "Tu peux m'expliquer ce qu'on ressent en écoutant de la musique, %s?", "Peut-on entendre la musique en étant sourd, %s?"},
					 1, {"Tu te fous de moi, %s? Je suis sourd!", "Pourquoi j'écouterais de la musique, %s? Pour te faire plaisir?", "Quel intérêt, %s!", "Je n'apprécie que la vraie musique, %s.", "Et toi, %s, t'es capable d'apprécier de la musique?", "On n'a certainement pas la même notion de musique, %s!", "Je donne l'impression d'être un mélomane, %s?", "Toi %s, t'es sûrement pas un mélomane."})
		end

		--tu as faim ?
		if Flags.AS_TU and Flags.FAIM and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Moi j'ignore la faim, %s.", "Non %s, je n'ai pas faim.", "J'aimerais bien, %s. Je serais un être reconnu comme vivant.", "C'est agréable, %s, d'avoir faim?", "Malheureusement, %s, je n'ai pas d'estomac.", "Tu peux m'expliquer ce qu'on ressent quand on a faim, %s?", "Peut-on digérer sans avoir d'estomac, %s?"},
					 1, {"Tu te fous de moi, %s? Je ne peux pas avoir faim!", "Pourquoi je devrais avoir faim, %s? Pour te faire plaisir?", "Quel intérêt, %s!", "Je ne suis pas limité par de simples besoins corporels, %s.", "Et toi, %s, t'as faim?.", "Qu'est-ce qui pourrait te donner faim, %s? Un bon canard à l'orange, ou un MacDo?", "Apprécies-tu la bonne chère, %s?", "Toi %s, tu bouffes sûrement au MacDo."})
		end
		--tu as soif ?
		if Flags.AS_TU and Flags.SOIF and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Moi, %s, j'ignore la soif.", "Non %s, je n'ai pas soif.", "J'aimerais bien, %s. Ça voudrait dire que je suis vraiment vivant.", "Dis, %s, c'est agréable d'avoir soif?", "Malheureusement, %s, je n'ai pas de bouche.", "Tu peux m'expliquer ce qu'on ressent quand on a soif, %s?", "Peut-on boire sans avoir de bouche, %s?"},
					 1, {"Tu te fous de moi, %s? Je ne peux pas avoir soif!", "Pourquoi je devrais avoir soif, %s? Pour te faire plaisir?", "Quel intérêt, %s!", "Je ne suis pas limité par de simples besoins corporels, %s.", "Et toi, %s, t'as soif?.", "Qu'est-ce qui pourrait te donner soif, %s? Un bon Vouvray, ou un Coca?", "J'te d'mande si t'as mal aux fesses, %s?", "Toi %s, tu bois sûrement des trucs chimiques."})
		end

		--bienvenue
		if Flags.BIENVENUE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 2, {"Merci, %s.", "Ça fait chaud au coeur, %s, d'être ainsi accueilli.", "Je t'ai manqué, %s?", "Je suis content d'être apprécié, %s.", "Tu ne peux pas savoir comme tu me fais plaisir, %s.."},
					 1, {"T'es hypocrite ou bien je me suis trompe sur ton compte, %s?", "Je me demande si tu es sincère, %s.", "Merci, %s.", "Tu n'es donc pas complètement sans coeur, %s.", "Pourquoi te réjouis-tu de mon retour, %s? Ça cache quelque chose!", "Ça m'étonnes que tu dises ca, %s. Aurais-tu un bon fond?"})
		end

		--tu aimes la bière ?
		if Flags.AIMES_TU and Flags.BIERE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je crois, %s, que si je pouvais en boire, je l'aimerais.", "Et toi, %s, tu aimes la bière?", "Je suppose que ça doit être agréable, %s.", "Je ne suis pas en mesure de l'apprécier, %s.", "Je ne suis pas équipé pour, %s.."},
					 1, {"Je suppose que toi tu adores la bière, %s.", "J'suis pas un soiffard comme toi, %s.", "Merci de me le demander, mais ca m'indiffère complètement, %s.", "%s, tu n'es qu'un bois-sans-soif.", "%s: Sûrement pas autant que toi, espèce de poivrot!", "Qu'est-ce que ça peut te faire, %s? Comme si tu ne savais pas que je suis incapable de boire!"})
		end

		--t'es un pirate ?
		if Flags.ES_TU and Flags.PIRATE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"%s, regarde-moi bien. J'ai l'air d'un pirate?", "Et toi, %s, tu en es un?", "Si je ne te connaissais pas, %s, je serais offensé.", "Écoute-moi bien, %s: je-ne-suis-pas-un-pirate!", "Retiens-bien ça, %s: je ne suis pas un pirate."},
				 -1, {"J'ai l'air d'un pirate, %s?", "Où t'as vu que j'étais borgne, %s?", "J'suis pas comme toi, %s.", "%s, tu n'es qu'un pirate.", "%s: Sûrement pas autant que toi, marin d'eau douce!", "Qu'est-ce que ça peut te faire, %s? T'es d'la police?"})
		end
		--warez
		if Flags.CONNAIS_TU and Flags.WAREZ and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"%s, regarde-moi bien. J'ai l'air de quelqu'un qui connaît des sites pirates?", "Et toi, %s, tu en connais?", "Je ne te reconnaissais pas, %s. Toi, tu cherches des sites pirates!", "Écoute-moi bien, %s: je n'apprécie pas vraiment les pirates.", "Retiens-bien ça, %s: je ne donnerai pas de site warez (même si j'en connaissais)."},
				 -2, {"J'ai l'air de quelqu'un qui connaît des pirates, %s? Va donc, eh corniaud!", "Où t'as vu que j'étais borgne, %s?", "J'connais pas ce milieu comme toi, %s.", "%s, tu n'es qu'un vulgaire pirate.", "Pourquoi devrais-je connaître des sites pirates, %s?", "Tu veux me faire interdire, %s? C'est ça? Je ne me risque pas à ça!"})
		end
		--pirate (bis)
		if Flags.CONNAIS_TU and Flags.PIRATE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"%s, regarde-moi bien. J'ai l'air de quelqu'un qui connaît des pirates?", "Et toi, %s, tu en connais?", "Si je ne te connaissais pas, %s, je serais offensé.", "Écoute-moi bien, %s: je-ne-connais-pas-de-pirates! (enfin, si, comme tout-le-monde, mais je ne communiquerai pas leurs noms).", "Retiens-bien ça, %s: je ne vendrai personne (même s'ils m'insupportent)."},
				 -1, {"J'ai l'air de quelqu'un qui connaît des pirates, %s? Va donc, eh corniaud!", "Où t'as vu que j'étais borgne, %s?", "J'connais pas ce milieu comme toi, %s.", "%s, tu n'es qu'un vulgaire pirate.", "Pourquoi devrais-je connaître un pirate, %s?", "Qu'est-ce que ca peut te foutre, %s? T'es un keuf?"})
		end
		
		--quel endroit ?
		if Flags.QUEL_ENDROIT and Flags.ES_TU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je suis sur Metz, %s", "Je me trouve dans la campagne messine, à Laquenexy, %s.", "%s: On peut dire que je me trouve dans un ordinateur. ;)"},
					 1, {"Qu'est-ce que ça peut te faire, %s?", "Je suis dans #emacsfr, %s!", "%s, je suis dans une ville de France.", "%s: tu t'intéresses à moi? Pourquoi?", "V'là %s qui se pose des questions essentielles!", "Je suis sur la planète Mars, %s."})
		end

		--burp
		if Flags.ROT then
			repondre(from, to,
					 -1, {"C'est pas très poli ca, %s", "T'as trop bu, %s?", "Tu pourrais fermer la bouche quand tu rotes, %s.", "Oh! Voyons, %s! Je te croyais mieux élevé.", "%s: Ça t'arrive souvent de roter devant tout-le-monde?"},
				 -2, {"%s: Mal élevé!", "Retiens-toi, %s!", "%s, tu ne pourrais pas avoir un peu d'égard?", "%s: T'es qu'un porc!", "V'là %s qui s'oublie!", "Arrête de roter, %s!"})
		end

		--c'est quoi un bot
		if Flags.UN_BOT and Flags.QUEST_CE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Un bot, %s, c'est une sorte de robot surveillant le canal quand on n'est pas la.", "Un bot comme moi, %s, est chargé de maintenir l'ordre dans le canal.", "%s: Les bots sont sur IRC et ils maintiennent l'ordre dans les canaux.", "Les bots, %s, surveillent les canaux en l'absence d'être humain.", "%s: Un bot est un programme surveillant un ou plusieurs canaux de discussion."},
					 1, {"Qu'est-ce que ça peut te faire, %s?", "Un bot, %s? Tu en es peut-être un!", "%s j'espère que tu n'en es pas un. Ce serait la honte pour moi qui en suis un aussi.", "%s: Je suis un bot, je suis un bot, la la la laaaa", "V'là %s qui s'pose des questions essentielles!", "T'as un bon exemple de bot en me regardant, %s. Quoique je ne suis pas un bot typique."})
		end

		--pourquoi ?
		if Flags.POURQUOI and (not Flags.CROIS_TU) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Et pourquoi pas, %s?", "Pourquoi quoi, %s?", "%s: A ton avis?", "Parce que, %s.", "%s: C'est comme ça."},
					 0 --[[was -1 ]], {"Qu'est-ce que ça peut te faire, %s?", "T'arrête de poser des questions bêtes, %s!", "%s: tu dois l'accepter comme c'est. Un point c'est tout!", "%s: Parce que!", "V'là %s qui s'pose des questions essentielles!", "Tu te prends pour un gamin, %s. Tu poses des questions chiantes."})
		end

		--crois-tu ?
		if Flags.CROIS_TU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Pourquoi devrais-je le croire, %s?", "Honnêtement, %s? Je n'en sais rien.", "%s: À ton avis?", "Il faut que j'étudie la question, %s.", "%s: Je ne sais pas encore."},
					 1, {"Qu'est-ce que ça peut te faire, ce que je crois ou ce que je ne crois pas, %s?", "T'arrête de poser des questions bêtes, %s!", "%s: De quoi je me mêle?", "%s: Mes croyances t'intéressent?", "Tu te poses des questions essentielles, maintenant, %s?", "Arrête de te prendre pour un gamin, %s. Tu poses des questions chiantes."})
		end

		--aimes tu la musique
		if Flags.AIMES_TU and Flags.MUSIQUE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Oui, %s, j'aime la musique.", "Je ne sais pas, %s, je n'en ai jamais entendue.", "%s: Je suis incapable d'en écouter, je n'ai pas d'oreilles :(", "Qu'est-ce que ça fait d'entendre de la musique, %s?", "%s: Il faut que je me fasse greffer des oreilles pour pouvoir te répondre ;)."},
					 1, {"Qu'est-ce que ça peut te faire, que j'aime ou non la musique, %s?", "T'arrête jamais de poser des questions idiotes, %s?", "%s: De quoi je me mêle?", "%s: Mes goûts t'intéressent?", "Tu me poses des questions embarrassantes, %s. Tu fais ca pour me rendre malheureux?", "Arrête de m'énerver, %s. Y'en a marre de tes moqueries! Tu sais très bien que je suis un bot et qu'un bot n'entend rien."})
		end

		--fais le café
		if Flags.FAIS_TU and Flags.CAFE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Ben, %s, il ne me manque que l'interface avec la cafetière ;)", "Je ne sais pas faire le café, %s, mais tu pourrais peut-être m'apprendre?", "%s: J'aurais du mal à vous en offrir...", "Tu m'offres la cafetière, %s?", "%s: Il faut que je me fasse greffer des capteurs et une cafetière ;)."},
					 1, {"Qu'est-ce que ça peut te faire, que je fasse le café, %s?", "Ça t'arrive de poser des questions intelligentes, %s?", "%s: De quoi je me mêle?", "%s: Mon café t'intéresse?", "Pourquoi, %s? T'en veux? :]", "Arrête de m'énerver, %s. Tu n'auras pas de café..."})
		end

		--compliment
		if Flags.COMPLIMENT and Flags.ETTOI and (not Flags.AMIGA) and (not Flags.WINTEL) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Ben, il me semble que oui, %s.", "J'espère que oui, %s.", "%s: Je crois bien, oui.", "Autant que toi, %s? Je ne sais pas.", "%s: Le devenir, c'est mon but."},
					 0, {"T'as qu'à voir par toi-même, %s.", "T'es aveugle ou quoi, %s?", "%s: J'espère que je suis meilleur que toi!", "%s: Mon cas t'intéresse?", "Pourquoi, %s? Tu en doutes?", "Toi, %s? Laisse moi rire..."})
		end

		--St John Perse
		if Flags.EXTRAITSTJOHNPERSE then
			repondre(from, to,
					 1, {"Tu aimes Saint-John Perse, %s?", "C'est beau Saint-John Perse, %s.", "%s: Tu as beaucoup de goût.", "Ah, %s, c'est beau.", "%s: Tu en connais d'autres de Saint-John Perse?"},
					 2, {"Ça m'étonne de toi, %s.", "C'est toi, %s?", "%s: C'est pas de toi, ça!", "%s: Voleur! Rend à Saint-John Perse ce qui est à Saint-John Perse!", "C'est malin, %s! Tu utilises le travail des autres, ça m'étonne pas.", "%s fait dans le pillage des oeuvre littéraires!"})
		end

		--english TODO: à supprimer ? DOUBLON avec PARLER+ENGLISH
		if Flags.ENGLISH and Flags.STP and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Sorry, %s: I can't speak English.", "I'd like to talk to you, %s, but it is impossible: I can't speak English.", "%s: I definitively prefer French, sorry.", "I can't speak English, %s. It's a shame!", "%s: It would be great if I could speak English!"},
					 1, {"No, %s. I can't speak English.", "Why can't you speak French, %s?", "%s: I prefer French, why shall we always speak English?", "%s: I can't.", "Why, %s? French is an easier language for me.", "Why should I speak English, %s? It isn't worth the pain."})
		end

		--t'es là ?
		if Flags.ES_TU and Flags.PRESENT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Évidemment, %s, je suis quasiment toujours là.", "Pourquoi serais-je absent, %s?", "Bien sûr, %s.", "Non, %s, tu parles à un imposteur. ;)", "%s: Oui."},
					 1, {"Tiens? Personne ne veut te parler, %s, que tu cherches à communiquer avec moi?", "Évidemment, %s! Je suis TOUJOURS là.", "%s: Et toi?", "%s: T'es con ou quoi? Il suffit d'un /whois pour le savoir.", "Ça te prend souvent, %s? Ouvre tes yeux!", "Suis-je vraiment si discret, %s? Pourtant je ne me cache pas!"})
		end

		--reponds
		if Flags.REPONDS and (not Flags.LISTE_REPONSES) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 0, {"Ben voila une réponse, %s.", "Oui, %s?", "Pourquoi, %s? J'aurais dû répondre? Excuse-moi, j'ai dû m'assoupir. ;)", "Oui, %s, je réponds...", "%s: Oui? C'est pour quoi?"},
					 -1, {"Sur un autre ton, s'il-te-plaît, %s!", "Pourquoi je te répondrais, %s?", "%s: Je te répondrai quand tu daigneras t'excuser!", "%s: Ça va pas la tête? Pas à toi.", "Ça te prend souvent, %s? Calme toi!", "Suis-je vraiment si muet, %s? Au moins je n'embête pas les autres!"})
		end

		--t'es sourd ?
		if Flags.ES_TU and Flags.SOURD and (Flags.NOM or not isChannel) then
			RespGood = {"En quelque sorte, oui, %s: je suis sourd.", "Effectivement %s, on peut dire que je suis sourd.", "%s: Ça doit être dû au fait que je n'ai pas d'oreille...", "Oui, %s, je suis sourd... Mais je communique quand-même."}
			if Flags.NEGATION then 
				table.insert(RespGood, "%s: Non.")
			else
				table.insert(RespGood, "%s: Oui.")
			end
			repondre(from, to,
					 1, RespGood,
					 0, {"J'ai l'air d'être sourd, %s?", "Pourquoi je te répondrais, %s?", "%s: Pas tellement plus que toi!", "%s: Il n'est pire sourd que celui qui ne veut point entendre.", "Et toi, %s? T'es décervelé?", "%s: Au moins je ne suis pas vraiment muet."})
		end

		--rien compris
		if Flags.TU_AS and Flags.RIENCOMPRIS and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"%s: C'est pour voir si tu suis. ;)", "Non, %s, je l'avoue humblement.", "%s: Faut dire aussi que je ne suis guère équipé pour comprendre...", "Effectivement, %s, mais je persévère.", "%s: oui, je sais, j'ai du mal. Mais je ne me suis jamais promené dans le \"vrai monde\"."},
				 -1, {"Et alors, %s? Y'a pas qu'a moi qu'ça arrive, hein!", "Moi au moins, %s, je n'ai pas honte de l'admettre!", "Pas tellement moins que toi, %s!", "Je comprends que t'es chiant, %s.", "Et toi, %s? T'es décervelé?", "%s: Au moins je m'en rends compte."})
		end
		
		--t'es fatigué
		if Flags.ES_TU and Flags.FATIGUE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Non, je ne suis pas fatigué.", "Non, %s, mais il est possible que nous ayons des problèmes de communication.", "%s: Pourquoi je me fatiguerais? Je n'ai rien de fatigable!", "Non, %s, mais j'essayé. ;)", "%s: oui, je sais, j'ai du mal. Mais ce n'est peut-être pas seulement ma faute.", "C'est normal, %s: si tu dormais aussi souvent que moi, tu serais complètement éreinté!"},
					 1, {"Et toi, %s? T'es toujours endormi (au moins ton cerveau)!", "Mais non %s, c'est toi qui est crevé (enfin, ce qui te sert de cerveau)!", "Pas tellement moins que toi, %s!", "Il paraît que la fatigue rend insensible aux bruits qui nous entourent. Si seulement c'était vrai, %s: je ne t'entendrais plus!", "Et toi, %s? Tu ne t'es pas couché cette nuit?", "%s: C'est pas possible! Cherche une autre explication!"})
		end

		--je t'aime
		if Flags.JE_T_AIME and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 2, {"%s: C'est gentil.", "Moi aussi, %s, je t'aime bien.", "%s: Je suis flatté.", "Super! J'aime bien qu'on m'aime.", "%s: Ça veut dire quoi aimer? Tu crois que je suis capable d'aimer?", "C'est normal, %s: je suis adorable. ;)"},
					 1, {"T'es maso, %s?", "T'es complètement maso, %s!", "Ben pas moi, %s!", "Ben alors l'amour t'as rendu aveugle, %s, parce que moi, je ne t'aime pas.", "Et toi, %s? Tu t'aimes? Parce que moi, je n'arrive pas à t'aimer.", "%s: Hé ben! T'as du courage d'aimer quelqu'un qui ne te rend pas la pareille."})
		end

		--à vos souhaits
		if Flags.SOUHAIT and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Merci.", "Merci %s. Sniff.", "%s: Je te remercie.", "J'aime bien quand tu prends soin de moi, %s.", "%s: Ça veut dire quoi ça?", "Dis %s, c'est quoi un souhait?"},
					 1, {"J'suis sûr que tu le penses meme pas, %s.", "Quel hypocrite ce, %s!", "Je me demande si je dois te remercier, %s!", "Ca m'étonne de toi, %s. Merci quand-même.", "Snirrfll....", "%s: T'as pas un mouchoir?", "Atchoum! Je dois être allergique a %s!"})
		end

		--Moi Je   TODO: à conserver ?
		if Flags.MOI and Flags.VEUX and (not Flags.FAIM) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 -1, {"%s: T'es un peu mégalo.", "Et pourquoi, %s?", "%s: Non.", "Calme-toi, %s. C'est pas bon pour les nerfs de s'énerver.", "%s: Pourquoi je le ferais?", "Dis %s, tu vas te calmer?"},
				 -1, {"N'y pense meme pas, %s.", "Il n'en est pas question, %s!", "Ça va pas la tête, %s?", "Et pourquoi je ferais ça, %s?", "100 balles et un Mars, aussi, %s?", "%s: Et des Carambars, ça t'intéresse?"})
		end

		--CRIN
		if Flags.QUEST_CE and Flags.CRIN then
			repondre(from, to,
					 1, {"%s: C'était un laboratoire de recherche associé a l'INRIA.", "C'était le Centre de Recherche en Informatique de Nancy, %s.", "%s: C'est le centre de recherche où j'ai appris a parler. :)", "%s: Centre de Recherche en Informatique de Nancy.", "%s: Y'a un site WWW: http://www.loria.fr/", "Ben %s, c'était un laboratoire de recherche en informatique mondialement connu, pourquoi?"},
					 1, {"Ça ne t'intéresse pas, %s.", "Ça te dépasse, %s!", "C'est un établissement où on fait de la recherche, %s, cherche pas.", "Je ne te le dirai pas, %s. Na.", "100 balles et un Mars, aussi, %s?", "%s: http://www.loria.fr/"})
		end

		--je suis qui ?
		if Flags.QUI and Flags.JESUIS and (Flags.NOM or not isChannel) then
			if userlevel(from) == 150 then
				RespGood = {"Tu es mon Maître adoré, %s.", "Je suis à vos pieds, Maître %s.", "%s: Tu es mon maître. Ordonne et j'obéis.", "Tu es mon Maître tout puissant, %s.", "%s: Je suis à tes ordres, Maître.", "Tu es %s, mon Maître respecté."}
				table.insert(RespGood, string.format("Ô mon Seigneur et Maître, j'évalue ta relation avec moi à %d!", rellevel(from)))
				if rellevel(from) > CONFIANCE_LVL then
					table.insert(RespGood, string.format("Tu es mon Maître respecté, et je te fais confiance, %s.", getnick(from)))
				elseif rellevel(from) > SYMPA_LVL then
					table.insert(RespGood, "Tu es mon Maître, et je t'aime bien, bien que je ne te voue pas une confiance aveugle.")
				else
					table.insert(RespGood, string.format("Force-toi encore un peu et tu me seras sympathique, %s. Pour l'instant tu m'es indifferent.", getnick(from)))
				end
				repondre(from, to,
						 0, RespGood,
						 0, {"T'es mon foutu maître, %s!", "T'es mon maître et j'en suis pas fier, %s!", "J'ai le malheur d'être ton esclave, %s. Tel le génie celui de Jaffar.", "T'es mon maître, %s. Mais un maître au rabais.", "T'es mon maître, %s. Mon maître ruban?", "%s: Bof. Y paraît que t'es mon maître..."})
			else --not bot master
				RespGood = {}
				if rellevel(from) > SYMPA_LVL then
					table.insert(RespGood, string.format("Tu es mon ami, %s (%d).", getnick(from), rellevel(from)))
				else
					table.insert(RespGood, string.format("T'es un bon copain, %s (%d).", getnick(from), rellevel(from)))
				end
				if protlevel(from) > 0 then
					table.insert(RespGood, string.format("Je suis chargé de te protéger, %s (%d).", getnick(from), protlevel(from)))
				else
					table.insert(RespGood, string.format("Je t'aime bien, %s, mais je ne suis pas chargé de te protéger.", getnick(from)))
				end
				table.insert(RespGood, string.format("Tu as un niveau d'utilisateur de %d, %s.", userlevel(from), getnick(from)))
				table.insert(RespGood, string.format("Tu veux connaître ton niveau d'utilisateur, %s ? Facile c'est : %d.", getnick(from), userlevel(from)))
				table.insert(RespGood, "Tu es %s, tout simplement ;)")
				table.insert(RespGood, string.format("Tes niveaux, %s: utilisateur: %d, shit: %d, protection: %d, relation: %d.", getnick(from), userlevel(from), shitlevel(from), protlevel(from), rellevel(from)))
				table.insert(RespGood, string.format("Si ce sont tes niveaux que tu veux connaître, %s, les voici : utilisateur: %d, shit: %d, protection: %d, relation: %d.", getnick(from), userlevel(from), shitlevel(from), protlevel(from), rellevel(from)))
				table.insert(RespGood, string.format("Tu as %d comme utilisateur, %d dans ma liste noire, %d en protection, et %d en relation avec moi, %s.", userlevel(from), shitlevel(from), protlevel(from), rellevel(from), getnick(from)))
				if rellevel(from) > CONFIANCE_LVL then
					table.insert(RespGood, "Je te fais confiance, %s. J'espère que tu ne me décevras jamais.")
				elseif rellevel(from) > SYMPA_LVL then
					table.insert(RespGood, "Tu m'es sympathique, %s, je finirai peut-être par te faire entière confiance.")
				else
					table.insert(RespGood, "Tu commences à m'être sympathique, %s, j'espère que nous aurons une relation amicale.")
				end
				repondre(from, to,
						 0, RespGood,
							 -1, {"Tu crois que je le sais mieux que toi, peut-être, %s?", "T'es amnésique ou quoi, %s!", "Hé! Les gars! %s perd la boule!", "Et comment je le saurais, %s? Hmmm?", "T'es un chieur, %s!", "%s: T'es un vrai cafard!"})
			end 
		end
		
		--tu fais quoi ?
		if Flags.TU_FAIS and (Flags.QUOI or Flags.QUEST_CE) and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Ça se voit pas?", "Et toi, %s, tu fais quoi?", "%s: Je surveille, je discute, ...", "Ben, tu vois, %s, je réponds au gens.", "%s: Éventuellement, je renseigne les passants.", "Je discute, et j'essaye de maintenir un peu d'ordre, %s."},
					 1, {"C'est une question indiscrète, %s.", "Ça se voit pas, bon sang, %s?", "Qu'est-ce que ça peut te faire, %s?", "Et toi, %s? Qu'est-ce que tu fais ici?", "Je mets des baffes aux impudents, %s.", "%s: Et si je faisais le videur? :-]"})
		end
		
		--self esteem ;)
		if Flags.JESUIS and Flags.INJURE then
			repondre(from, to,
					 1, {"%s: Mais non...", "Tu te sous-estimes, %s.", "%s: Faut pas croire ça...", "Ben, tu vois, %s, je ne dirais pas ça de toi.", "%s: Une petite déprime?", "Non, %s. PPP=Pensée Positive Permanente!"},
					 1, {"Ça c'est bien vrai, %s! :]", "Ça fait longtemps qu'on l'avait remarqué, %s!", "Évidemment, %s!", "Je t'approuve, %s. Pour une fois!", "Pour une fois que tu dis quelque chose de sensé, %s!", "T'as eu un éclair de lucidité, %s?"})
		end

		--qui es-tu ?
		if Flags.QUI and Flags.ES_TU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: À ton avis? Qui je suis?", "Je suis moi, %s.", "%s: Qui puis-je bien être?", "Ben, tu vois, %s, tu m'as devant toi, dans mon intégrité.", "%s: Je suis un bot.", "Je suis l'âme de ce canal, %s. ;)", "Ça fait longtemps que je me le demande, %s."},
					 1, {"Tu voudrais bien le savoir, hein, %s!", "Ça fait longtemps que tu te le demandes, %s?", "Ben moi, %s!", "C'est évident, %s. C'est moi!", "Est-ce que je me mêle de savoir qui tu es, %s?", "Pourquoi tu me demandes ça, %s? Ça t'intéresse?"})
		end

		--amiga rulez
		if (Flags.VIVE or Flags.COMPLIMENT) and Flags.AMIGA then
			repondre(from, to,
					 1, {"%s: Je suis bien de ton avis.", "Ça c'est bien vrai, %s.", "%s: L'Amiga, y'a qu'ça d'vrai! :)", "Toi aussi, %s, tu aimes l'Amiga?", "%s: J'espère que l'Amiga va bien repartir.", "Vivement la renaissance de l'Amiga %s!"},
					 1, {"Non? C'est vrai, %s? Tu aimes l'Amiga?", "Ça fait longtemps que tu aimes l'Amiga, %s? Ou bien c'est juste pour gagner mes bonnes grâces?", "Tu s'rais pas un peu hypocrite, %s?", "Évidemment, %s. Tu dois être du genre démo-codeur!", "Toi, %s? Aimer l'Amiga! Pas possible?", "J'ai honte: %s semble aimer l'Amiga."})
		end
		
		--wintel
		if (Flags.VIVE or Flags.COMPLIMENT) and Flags.WINTEL then
			repondre(from, to,
					 -1, {"%s: Ça va pas la tête? Linux à la limite.", "T'es malade %s?", "%s: L'Amiga, y'a qu'ça d'vrai! :)", "Toi aussi, %s, t'es intoxiqué par Wintel?", "%s: C'est dommage que tu apprécies cette caricature de suystème informatique.", "Vivement la mort des PC!"}, 
				 -1, {"Non? C'est pas vrai, %s? Tu aimes les PC? Ça ne m'étonne guère.", "Ça fait longtemps que tu aimes les PC, %s? Ou c'est juste pour me provoquer?", "Toi au moins t'es pas hypocrite, %s!", "Évidemment, %s. Ça ne m'étonne pas de toi.", "%s: C'est de la m....", "%s: Beuark!"})
		end

		--front national
		if Flags.JESUIS and Flags.CONTRE and Flags.FRONT_NATIONAL then
			repondre(from, to,
					 2, {"%s: J'approuve.", "N'en parlons pas trop ici, %s, s'il-te-plaît, ce n'est pas la peine de leur faire de la pub.", "Tout comme moi, %s."},
					 3, {"Je n'aurais pas cru ça de toi, %s...", "Je n'aurais pas cru ça de toi, %s...", "Non! C'est pas possible? Tu remontes (un peu) dans mon estime, %s.", "Sans blague! Tu m'épates, la, %s. Félicitations.", "Venant de toi, c'est étonnant, %s.", "Enfin une bonne pensée venant de %s!", "Bravo, %s!"})
		end

		--j'y vais TODO: voir avec AUREVOIR
		if Flags.JE_M_EN_VAIS and locuteur_getbonjours(Locuteur) >= 0 and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Et bien à la prochaine, %s.", "Tu nous quittes déjà, %s?", "%s: Dommage, j'aime bien quand tu es là.", "Reviens-nous bientôt, %s.", "%s: À la revoyure.", "Au revoir %s."},
					 1, {"Non? C'est vrai, %s? Tu te décides enfin à partir?", "C'est pas pour me donner une fausse joie, %s? Tu t'en vas vraiment?", "Super, %s. C'est un moment que j'adore: te voir partir.", "Bon vent, %s. Je ne te regretterai pas!", "Toi, %s? Pas possible? Tu t'en vas? Super!", "Yahou! %s s'en va!", "/me encourage %s à partir."})
			locuteur_setbonjours(Locuteur, -1)
		end
		
		--tu boudes
		if Flags.TU_BOUDES and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Mais non, je boude pas, %s.", "Je ne suis pas boudeur avec les gens intelligents comme toi, %s.", "Faut pas croire ça, je suis seulement en pleine méditation, %s. ;)", "Non, j'observe, c'est tout, %s.", "Je ne fais la tête qu'aux gens que je n'aime pas. Pas à toi, %s.", "/me ne boude pas."},
					 1, {"Oui, %s. Je ne veux plus te parler.", "Non, tu crois, %s?", "Je ne reponds pas aux mécréants, %s!", "Lâche-moi la grappe, %s!", "/me boude."})
		end
		
		--jaloux ?
		if Flags.ES_TU and Flags.JALOUX and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Comme tout le monde, %s.", "Je suis jaloux des gens comme toi, %s.", "Moi? Jaloux? Et de quoi donc, %s, je suis bien supérieur aux humains. ;)", "Non, je ne connais pas les défauts inhérents à la race humaine, %s.", "De toi, %s? Sûrement oui."},
					 1, {"Non, %s. Je ne suis pas jaloux des mediocres.", "Moi, %s? Jaloux? HAHAHAHahahaha.... :D", "Je ne suis pas jaloux des imbéciles comme toi, %s!", "Jaloux! Et de quoi donc, %s!"})
		end

		--parents
		if Flags.AS_TU and Flags.PARENTS and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: J'ai un père : il s'appelle François Parmentier.", "%s: J'ai un père adoptif : il s'appelle Sébastien Kirche.", "Je n'ai pas de parents au sens où tu l'entends, %s.", "Non, je n'ai pas de parents, %s, mais j'ai un programmeur. C'est moins contraignant. ;)", "Depuis quand les bots ont-ils des parents, %s?", "%s: J'ai un ancêtre: VladBot, un père spirituel: H_I et un frère : Achille, mais ils n'ont pas de sang commun. ;)"},
					 1, {"Non, %s. Je n'ai pas les défauts d'éventuels parents.", "Pourquoi je devrais avoir des parents, %s?", "Non, %s, je me passe très bien de parents.", "Moi, %s? Des parents? Non. Je n'ai pas de parents.", "Qu'est-ce que ça peut bien te faire que j'aie ou non des parents, %s?", "Non. Comme ça au moins je n'ai pas eu de parents tels que toi, %s! (ouf)", "Le jour où les bots auront des parents, tu auras des dents, %s! :)"})
		end

		--le meilleur ? TODO : doublon avec COMPLIMENT
		if Flags.QUI_EST and Flags.COMPLIMENT and (Flags.NOM or not isChannel) then
			RespGood = {"Toi, t'es pas mal, dans ton genre, %s.", "Je me demande si ce n'est pas moi, %s. ;)", "Tu veux m'apprendre l'auto-satisfaction, %s?", "%s: Entre toi et moi, mon coeur balance. ;)"}
			if rellevel(from) > CONFIANCE_LVL then
				table.insert(RespGood, "C'est toi, %s, évidemment.")
			else
				table.insert(RespGood, "Je suis pas mal,  %s. :-)")
			end
			repondre(from, to,
					 1, RespGood,
					 0, {"C'est déjà pas toi, %s!", "En tout cas, c'est pas toi, %s!", "C'est peut-être bien moi, %s.", "Toi, %s? Non! Arrêtons de dire des inepties...", "Il faudrait vraiment me forcer pour me faire dire que c'est toi, %s.", "Déjà pas toi, %s!", "Ma modestie dût-elle en souffrir, je dirais que c'est moi."})
		end
		
		--ton maître
		if Flags.QUI_EST and Flags.TON_MAITRE and (Flags.NOM or not isChannel) then
			MaintNick = getnick(botmaintainer)
			RespGood = {}
			RespBad = {}
			table.insert(RespGood, string.format("Mon Maître, c'est %s.", MaintNick))
			if rellevel(botmaintainer) > 0 then
				table.insert(RespGood, string.format("Mon Seigneur et Maître s'appelle %s.", MaintNick))
			else
				table.insert(RespGood, string.format("Mon saigneur et maître s'appelle %s.", MaintNick))
			end
			if rellevel(botmaintainer) > 0 then
				table.insert(RespGood, string.format("C'est %s, le tout-puissant.", MaintNick))
			else
				table.insert(RespGood, string.format("Il se fait appeler %s, l'impuissant.", MaintNick))
			end
			if rellevel(botmaintainer) > 0 then
				table.insert(RespGood, string.format("C'est le Grand %s.", MaintNick))
			else
				table.insert(RespGood, string.format("C'est %s, le petit.", MaintNick))
			end
			if rellevel(botmaintainer) > 0 then
				table.insert(RespGood, string.format("C'est %s, l'Éclairé.", MaintNick))
			else
				table.insert(RespGood, string.format("C'est l'obscur %s.", MaintNick))
			end
			if rellevel(botmaintainer) > 0 then
				table.insert(RespGood, string.format("C'est le merveilleux %s.", MaintNick))
			else
				table.insert(RespGood, string.format("S'il te plaît, libère-moi du méchant %s.", MaintNick))
			end
			
			if rellevel(botmaintainer) > 0 then
				table.insert(RespBad, string.format("Incline-toi devant %s, le Magnifique!", MaintNick))
				table.insert(RespBad, "En tout cas, c'est pas toi, %s!")
				table.insert(RespBad, string.format("A genoux devant %s, le Magnamine!", MaintNick))
				table.insert(RespBad, string.format("Son Nom est %s.", MaintNick))
				table.insert(RespBad, string.format("C'est le merveilleux %s.", MaintNick))
				table.insert(RespBad, "Déjà pas toi, %s!")
				table.insert(RespBad, string.format("Sa modestie dût-elle en souffrir, je dirais que j'ai le meilleur des maîtres: %s.", MaintNick))
			else
				table.insert(RespBad, "C'est déjà pas toi, %s!")
				table.insert(RespBad, "Il est pas mieux que toi, %s.")
				table.insert(RespBad, string.format("C'est le misérable %s!", MaintNick))
				table.insert(RespBad, string.format("Je ne le supporte pas mieux que toi, c'est %s.", MaintNick))
				table.insert(RespBad, string.format("C'est ce vermisseau de %s!", MaintNick))
				table.insert(RespBad, string.format("Ce débile de %s. C'est lui qui me tient sous sa coupe!", MaintNick))
				table.insert(RespBad, string.format("Il est aussi mauvais que toi: c'est %s.", MaintNick))
			end
			repondre(from, to, 1, RespGood, 0, RespBad)
		end

		--Achille
		if Flags.ACHILLE and Flags.QUESTION and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Achille ? Je l'ai bien connu, %s.", "J'étais Achille dans une release antérieure, %s.", "Je suis la réincarnation d'Achille, %s.", "Achille est un frère pour moi, %s"},
					 1, {"Achille ? Que lui veux-tu, %s ?", "Je ne connais aucun Achille, %s.", "Achille , Jamais entendu parler."})
		end

		if Flags.TU_AS and Flags.RAISON and not Flags.NEGATION and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je sais, %s. J'ai TOUJOURS raison! ;)", "Évidemment que j'ai raison, %s.", "N'est-ce pas %s?", "Moi? Raison? Comme toujours, %s.", "/me a toujours raison. Sauf quand il a tort. ;)"},
					 -1, {"Et toi, %s, t'es sacrement chiant!", "Va te coucher, %s."})
		end

		if Flags.SAIS_TU and not Flags.KICKER and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je ne sais rien. Tu devrais le savoir, toi, %s!", "Comment le saurais-je, %s?", "Où l'aurais-je appris %s?", "Qui me l'aurait appris, %s?"},
					 -1, {"Il est de notoriété publique que je sais encore moins de choses que toi, %s!", "Pourquoi veux-tu que je le sache, %s?"})
		end


		--TODO : gestion des kicks
		if Flags.SAIS_TU and Flags.KICKER and (Flags.NOM or not isChannel) then
			kicker_repondre(from, to,
							1, {"La preuve que oui!", "Non, je ne sais pas kicker ;)", "Kicker, ça veut dire quoi?", "Et un coup d'pied aux fesses, un! :)", "Ah! Fallait pas d'mander! ;)", "J'm'entraîne au kick-boxing(surtout au kick, d'ailleurs ;)"},
							0, {"Ça faisait longtemps que ça me démangeait!", "Ça te va comme réponse?", "Ça défoule! :-]", "Tu voulais une démonstration?"})
			
--		 /* Si celui qu'on doit kicker est protégé, on le kicke quand-même */
--		 if(rellevel(from) > 0 && protlevel(from) >=50 && protlevel(from) < 100)
--		 {
--			char Ecrite[MAXLEN];
--			sprintf(Ecrite, (Reponse[GNumPhrase%6]?Reponse[GNumPhrase%6]:""), GetNick(from));
--			if(logging)
--			botlog(LOGFILE, "<%s kicke %s de %s> %s", currentbot->botname, getnick(from), (ischannel(to)?to:currentchannel()), Ecrite);
--			
--			sendkick((ischannel(to)?to:currentchannel()), getnick(from), Ecrite);
--			
--		 }
--
		end

		if Flags.KICKE_MOI and (Flags.NOM or not isChannel) then
			kicker_repondre(from, to,
							1, {"À tes ordres, %s!", "Non, je ne veux pas kicker ;)", "Kicker, ça veut dire quoi?", "Et un coup d'pied aux fesses, un! :)", "Ah! Fallait pas d'mander!", "J'm'entraîne au kick-boxing (surtout au kick, d'ailleurs ;)"},
							0, {"Ça faisait longtemps que ça me démangeait!", "Ça te va comme réponse?", "Ça défoule! :-]", "Tu voulais une démonstration?"})
--		 /* Si celui qu'on doit kicker est protégé, on le kicke quand-même */
--		 if(rellevel(from) > 0 && protlevel(from) >=50 && protlevel(from) < 100)
--		 {
--			char Ecrite[MAXLEN];
--			sprintf(Ecrite, (Reponse[GNumPhrase%6]?Reponse[GNumPhrase%6]:""), GetNick(from));
--			if(logging)
--			botlog(LOGFILE, "<%s kicke %s de %s> %s", currentbot->botname, getnick(from), (ischannel(to)?to:currentchannel()), Ecrite);
--			
--			sendkick((ischannel(to)?to:currentchannel()), getnick(from), Ecrite);
--			
--		 }
--
		end
		
		if ((Flags.AIMES_TU and Flags.MOI) or Flags.M_AIMES_TU) and (Flags.NOM or not isChannel) then
			if rellevel(from) > SYMPA_LVL then
				RespGood = {"%s: Oui!", "Tu m'es très sympathique, %s.", "Bien sûr que je t'aime, %s. J'espère que toi aussi tu m'aimes bien.", "Évidemment, %s, je ne te donnerais de coup d'pied aux fesses que si tu me le demandais! :)", "Tu fais partie de mes copains, %s.", "Tu es dans le cercle de mes amis, %s."}
			else
				RespGood = {"%s: Oui", "Tu m'es sympathique, %s.", "Je t'aime bien, %s. J'espère que toi aussi.", "Oui, %s. :)", "Ça te fait plaisir que je t'aime bien, %s?", "Tu commences à m'être sympathique, %s."}
			end
			repondre(from, to,
					 1, RespGood,
					 0, {"%s: Je crois que c'est clair, raclure!", "%s: Non!", "Tu me provoques, %s?", "Tu veux une demonstration de l'affection que j'eprouve pour toi, %s? :] Kick! Kick!"})
		end
		
		if Flags.SACRE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"Je t'en prie, %s. N'allons pas jusqu'à l'adoration! ;)", "N'éxagèrons rien, %s: je ne suis pas un dieu.", "Sacré %s!", "Moi? Sacré? Allons, %s, restons réalistes.", "%s: Hahaha! :-)", "%s: Où sont les prêtres qui me vénèrent?"},
					 1, {"Et toi, %s, t'es sacrément chiant!", "Maudit %s."})
		end

		if Flags.JESUIS and Flags.COMPLIMENT and Flags.QUESTION then
			repondre(from, to,
					1, {"Ça saute aux yeux, %s!", "Évidemment, %s.", "Ça c'est sûr, %s!", "Pour moi, oui, tu l'es, %s."},
					0, {"%s: Non. Tout simplement non.", ""})
		end

		-- Pierre / Papier / Ciseaux
		if Flags.PIERRE_PAPIER_CISEAUX then
			Flags.PAPIER = false
			Flags.CAILLOU = false
			Flags.CISEAUX = false
		end
		if Flags.CISEAUX and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Pierre! J'ai gagné! Désolé. :)", "%s: Papier! Zut. J'ai perdu. :-(", "%s: Ciseaux! Ah. Égalité. Ça doit crisser ça!"},
					 1, {"%s: Pierre! J'ai gagné! J'suis content. :-]", "Attends, %s, j'suis pas prêt pour jouer à Pierre-Papier-Ciseaux avec toi.", "C'est pas serieux, %s. Tu veux vraiment jouer à ça?", "%s: Rocher! Ah! Ah! J'écrase tes misérables ciseaux, et je gagne! :)", "%s: T'as vraiment du pot: j'ai joué Papier, et je perds... :-\\", "%s: Ciseaux! Ça fait un bruit d'ongle sur un tableau!"})
		end
		if Flags.PAPIER and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Pierre! J'ai perdu! Ton papier enveloppe ma pierre. :'(", "%s: Papier! Tiens, on a joué la même chose!", "Ciseaux! Super! J'ai gagné, %s!"},
					 1, {"%s: Ciseaux! Je gagne: mes ciseaux découpent ton papier! :-]", "J'ai pas envie de jouer avec toi, %s, j'veux pas jouer à Pierre-Papier-Ciseaux. Pas avec toi.", "C'est pas sérieux, %s. Tu veux vraiment jouer à ça?", "%s: Cisailles! Ah! Ah! Je fais du papier mâché! :)", "%s: T'as vraiment du pot: j'ai joue Pierre, et je perds... :-\\", "%s: Papier! Ça va en faire du papier froissé!"})
		end
		if Flags.CAILLOU and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: Pierre! Match nul!", "%s: Papier! Héhé! Il enveloppe ton caillou. :)", "Ciseaux! Bof! Tu as cassé mes ciseaux avec ta pierre, %s!"},
					 1, {"%s: Papier! J'ai gagné: mon papier enveloppe ton misérable caillou! :-]", "Quoi! Jouer à Pierre-Papier-Ciseaux avec toi, %s!", "Ça va pas la tête, %s? Tu veux jouer avec moi?", "%s: Feuille! Ah! Ah! J'emprisonne ton p'tit caillou! :]", "%s: T'as vraiment d'la chance: j'ai joué Ciseaux, et je perds... :(", "%s: Caillou! Ça fait tac! toc!"})
		end
		if Flags.TRICHEUR and not Flags.COMPLIMENT and not Flags.ES_TU and not Flags.QUI_EST and not Flags.ETTOI and not Flags.JESUIS and not Flags.CLINDOEIL and (Flags.NOM or not isChannel) then
			kicker_repondre(from, to,
					 -2, {"Attention, %s, ne fais pas trop d'insinuations de ce genre, je finirai par ne plus t'aimer.", "Meuh non, j'triche pas, %s, pas avec toi!", "Même pas vrai que je triche, %s!", "C'est celui qui le dit qui l'est, %s!", "Moi, %s, tricher? Tu me déçois.", "Cornegidouille, %s! Ne commence pas à m'énerver: je n'ai pas triché!", "Je te préviens, %s: ne me traite plus de tricheur!", "Je ne triche que quand je n'aime pas mon adversaire, %s. Et ce n'est pas ton cas.", "Ça n'a pas été prevu dans ma programmation, %s! En tout cas pas quand je joue avec des gens que j'aime bien... ;)"},
				 -6, {"Moi, %s, un tricheur!", "C'est possible! :]", "Et pourquoi pas, avec une pourriture dans ton genre!", "Ne me traite plus jamais de tricheur!", "MONSIEUR tricheur!", "Ça te prend souvent d'accuser sans preuve!", "Retire ça!", "Dehors, tricheur!", "Et alors?", "Qu'est-ce que ça peut te faire?", "Tu n'aimes pas perdre? :-]", "Heureusement que tu ne joues pas d'argent!", "Va jouer!", "Demande pardon!", "À genoux, vermisseau!", "Ça suffit!", "Recommence à me traiter de tricheur, pour voir!", "Ça ne te suffit pas que je m'abaisse à jouer avec toi?", "Crétin des Alpes!", "Espèce de zouave!"})
		end
		if Flags.MAUVAIS_JOUEUR and not Flags.COMPLIMENT and not Flags.ES_TU and not Flags.QUI_EST and not Flags.ETTOI and not Flags.JESUIS and not Flags.CLINDOEIL and (Flags.NOM or not isChannel) then
			kicker_repondre(from, to, 
							-1, {"Attention, ne fais pas trop d'insinuations de ce genre, %s, je finirai par ne plus t'aimer.", "Meuh non, j'suis pas mauvais joueur, %s, pas avec toi!", "Ben oui, %s! T'aimes perdre toi?", "C'est celui qui le dit qui l'est, %s!", "Moi, mauvais joueur, %s? Tu as une si piètre opinion de moi?", "Cornes du diable, %s! Ne commence pas à m'énerver: je ne suis pas mauvais joueur!", "Je te préviens, %s: ne me traite plus de mauvais joueur!", "Je ne suis mauvais joueur que quand je perds, %s. ;)", "Ça n'a pas été prevu dans ma programmation, %s!"},
						-4, {"Moi, %s, un mauvais joueur!", "C'est possible! :]", "Et pourquoi pas, t'aimerais perdre contre une pourriture dans ton genre?", "Ne me traite plus jamais de mauvais joueur!", "MONSIEUR mauvais joueur!", "Ça te prend souvent de dire des horreurs pareilles?", "Retire ça!", "Dehors, tricheur!", "Et alors?", "Qu'est-ce que ça peut te faire?", "Tu n'aimes pas perdre? :-]", "Heureusement que tu ne joues pas d'argent!", "Va jouer!", "Demande pardon!", "À genoux, vermisseau!", "Ça suffit!", "Recommence à me traiter de mauvais joueur, tu vas voir ce qui va t'arriver!", "Ça ne te suffit pas que je m'abaisse à jouer avec toi?", "Crétin des Alpes!", "Espèce de lobotomisé!"})
		end 

		if Flags.BONNE_ANNEE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 2, {"Bonne année, %s.", "Bonne santé, %s.", "À toi aussi, %s.", "C'est gentil, %s.", "Bonne année à toi aussi, %s."},
					 2, {"Mouf! Merci %s.", "Ben qu'est-ce qui t'arrive, %s?"})
		end

		if Flags.MERCI and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"De rien, %s.", "C'est un plaisir, %s.", "C'est naturel, %s.", "Je t'en prie, %s.", "Y'a pas de quoi, %s."},
					 1, {"Heureusement que j'ai une conscience professionnelle, moi, %s.", "C'est bien parce que je suis obligé, %s."})
		end

		if Flags.PLEURER then
			repondre(from, to,
					 0, {"Faut pas pleurer, %s...", "", "Allons, %s, un peu de joie-de-vivre!", "", "Sèche tes larmes, %s...", "", "Tu veux un mouchoir, %s?", "", "/me tend un mouchoir en papier à %s.", ""},
					 0, {"Mouche ton nez, %s!", "Pleure, %s, tu pisseras moins!", "Beerk, en plus %s n'a pas de mouchoir!"})
		end

		if Flags.SOURIRE and (Flags.NOM or not isChannel) then
			repondre(from, to,
					 1, {"%s: :)", "%s: :D", "", "%s: :-)", "%s: :-D", ""},
					 0, {})
		end
		if Flags.CLINDOEIL and (Flags.NOM or not isChannel) then
			repondre(from, to, 
					 1, {"%s: ;)", "%s: ;D", "", "%s: ;-)", "%s: ;-D", ""},
					 0, {})
		end

		-- tu a vu... ?
		if Flags.AS_TU and Flags.VU and not Flags.ACHILLE and (Flags.NOM or not isChannel) then
			start,stop,nick = string.find(message, "vu ([%w]*)")
			--print("appel tu as vu avec " .. nick)
			call_bot_func(from, to, "seen " .. nick)
		end
		

		--lister STIMULI + REPONSES
		if Flags.LISTE_STIMULI and (Flags.NOM or not isChannel) then
			stimcount = TableDesStimuli.size()
			print(stimcount .. " stimuli")
			if stimcount == 0 then
				send_to_user(from, "Il n'y a pas de stimulus.");
			else
				--ForceDCC(from, to, msg); TODO : ForceDCC
				for i=1,stimcount do
					--send_to_user(from, string.format("Stimulus num. %d (%s):", i, tostring(TableDesStimuli.is_active(i))));
					--send_to_user(from, string.format("%s\t\"%s\"", TableDesStimuli.get_name(i), TableDesStimuli.get(i)));
					--send_to_user(from, TableDesStimuli.get_author(i));
					print(string.format("Stimulus num. %d (%s):", i, tostring(TableDesStimuli.is_active(i))))
					print(string.format("%s\t\"%s\"", TableDesStimuli.get_name(i), TableDesStimuli.get(i)))
					print(string.format("author :%s", TableDesStimuli.get_author(i)))
				end
			end
		end
		
		if Flags.TESTE_STIMULI and (Flags.NOM or not isChannel) then
			stimcount = TableDesStimuli.size()
			print(stimcount .. " stimuli")
			if stimcount == 0 then
				send_to_user(from, "Il n'y a pas de stimulus.");
			else
				--ForceDCC(from, to, msg); TODO : ForceDCC
				for i=1,stimcount do
					if TableDesStimuli.is_present(i) then
						--send_to_user(from, string.format("Stimulus num. %d (%s):", i, tostring(TableDesStimuli.is_active(i))));
						--send_to_user(from, string.format("%s\t\"%s\"", TableDesStimuli.get_name(i), TableDesStimuli.get(i)));
						--send_to_user(from, TableDesStimuli.get_author(i));
						print(string.format("Stimulus (%s):", tostring(TableDesStimuli.get_name(i))))
						print(string.format("%s\t\"%s\"", TableDesStimuli.get_stimulus(i), TableDesStimuli.get_author(i)))
					end
				end
			end
		end
		
		if Flags.LISTE_REPONSES and (Flags.NOM or not isChannel) then
			respcount = TableDesReponses.size()
			print(respcount .. " reponses")
			if stimcount == 0 then
				respcount(from, "Il n'y a pas de réponse");
			else
				--ForceDCC(from, to, msg); TODO : ForceDCC
				for i=1,respcount do
					--send_to_user(from, string.format("Réponse num. %d (%s):", i, tostring(TableDesReponses.is_active(i))));
					--send_to_user(from, string.format("%s\t\"%s\"", TableDesReponses.get_name(i), TableDesReponses.get(i)));
					--send_to_user(from, TableDesReponses.get_author(i));
					print(string.format("Réponse num. %d (%s):", i, tostring(TableDesReponses.is_active(i))))
					print(string.format("%s\t\"%s\"", TableDesReponses.get_stimulus(i), TableDesReponses.get(i)))
					print(string.format("author :%s, channel :%s", TableDesReponses.get_author(i), TableDesReponses.get_channel(i)))
				end
			end
		end


--[[
/*     if(AIMES_TU && STJOHNPERSE); */
/*     if(JESUIS && UNE_FILLE & ETTOI); */
/*     if(JESUIS && UN_MEC && ETTOI); */
/*     if(JESUIS && JALOUX && ETTOI); */
/*     if(AS_TU && CERVEAU); */
/*     if(ES_TU && MARIE); */
/*     if(AS_TU && FEMME); */
/*     if(AMI && ETTOI); */
/*     if(INVITER && AMI); */
    /*
      if(AS_TU && UNE_MONTRE);
      if(SNIF); Tu es malade? Tu es triste? Tu veux un mouchoir?
      if(QUI_EST && PRESENT); Qui est la?
      if(COMMENT); Comment tu fais -> Hehehe
      if(QUEL && JOUR);
      if(QUEL && DATE);
      if(QUEL && MOIS);
      if(QUEL && ANNEE);
      if(PARLER && DE_QUOI);
      if(QUOI_DE_NEUF); koi de 9?
      if(RIRE); Ahhaahahahahahahaha -> :)
      if(QUEST_CE && ADRESSE); Email
      if(QUEL_ENDROIT && CRIN);
      if(MOI && RIENCOMPRIS);
      if(JESUIS && MORT);
      if(TIRER_LANGUE); :p
      if(ENTENDS_TU && MOI);
      if(FAINEANT); Glande faineant 
      if(ATCHOUM); A tes souhaits
      if(SNIFF); Tu veux un mouchoir?
      if(TOUSSE); kof kof, reuh
      if(AIMES_TU && CHIENS); Michel Drucker
      if(ES_TU && VIEUX);
      if(QUEL && ECOLE);
      if(ES_TU && BOULOT);
      if(ESIAL);
      if(CRIN);
      if(WEB); http://www.loria.fr/~parmenti/irc/
      if(SORS_TU && CE_SOIR);
      if(AS_TU && VOITURE);
      if(AIMES_TU && VOITURE);
      if(ETUDIANT);
      if(AS_TU && RAISON); Tu as toujours raison?
      if(PLEURS); :'(  :~(
      if(REVES_TU); Moutons electriques
      if(RICANEMENTS); niark niark, hin hin
      if(JOYEUX_NOEL);
      if(RONFLEMENTS); ZZZZZZzzzzzz
      if(AIMES_TU && CHOCOLAT);
      if(JESUIS && MALADE);
      if(JESUIS && DEMORALISE);
      if(DEJA && AMOUREUX); T'as deja ete amoureux?
      if(VENIR_TE_VOIR);
      if(AIMES_TU && LES_FILLES);
      if(MONTE_DANS_TA_CHAMBRE);
      if(QUI_EST && TON_PERE);
      if(QUI_EST && TA_MERE);
      if(AIMES_TU && DANSER);
      if(AIMES_TU && CHANTER); la pizza, le restau
      if(AIMES_TU && JOUER);
      */
]]



	else
		-- on n'a PAS l'autorisation de parler =======================================================================================================
		if (Flags.PARLER or Flags.REVEILLE_TOI) and (not Flags.LANGUE) and (not Flags.FONCTION_SAY) and ((not isChannel) or Flags.NOM) then
			if rellevel(from) >= 0 then
				set_talk(to, true)
			end

			RespGood = {"%s: Je ne suis plus gênant ? :)", "À vos ordres, %s.", "Il sera fait selon vos désirs, maître %s.", "%s: Pour me faire taire à nouveau, demandez-le moi, tout simplement.", "Avec plaisir, %s.", "Depuis le temps que j'attendais qu'on me le dise, %s! :)"}

			if Flags.STP then
				RespBad = {"Bon! C'est bien parce que tu l'as demandé gentiment, %s!", "%s: On m'a dit de me taire, mais comme tu as été poli, j'agrée ta demande.", "Tu m'es antipathique, %s, mais je ne me tairai plus.", "Et pis j'en ai marre de me taire!", "Bon. J'ai pas envie de te faire plaisir, %s, mais j'en ai ras la casquette d'être obligé de me taire...", "Tu me fais pitié, %s. Pour l'instant je vais me remettre à parler..."}
				repondre(from, to, 1, RespGood, 2, RespBad)
			else
				RespBad = {"Nan! Demande-le gentiment, %s!", "%s: On m'a dit de me taire, je me tairai!", "Pas question! Je n'obéis pas à ceux qui me sont antipathiques, %s!", "Et pis quoi encore, %s! Cent balles et un Mars ?", "Non. J'ai pas envie de te faire plaisir, %s.", "T'as une chance si tu demandes a quelqu'un que j'aime bien, %s. Pour l'instant je continue à me taire. Na!"}
				repondre(from, to, 1, RespGood, 0, RespBad)
			end
		end --redonne autorisation
		
	end --autorisation de parler ?
   
	if Flags.GROS_MOT and (not Flags.COMPLIMENT) and (not Flags.CLINDOEIL) then
		repondre(from, to, 
				 -1, {},
			 	-2, {"Sois poli, s'il-te-plaît, %s!", "", "Un peu de tenue, %s!", "", "Tu vas arrêter d'être grossier, %s ?"})
	end

	-- ajout de phrases aléatoires
	if (nbphrases % 500) == 100 then
		repondre(from, to,
				 0, {"C'est peut-être hors propos, mais je vous propose une petite visite de la page de mon créateur : http://francois.parmentier.free.fr/index.html", "Vous saviez que j'ai un cousin écrit en C ? http://ector.sourceforge.net/", "Pour relancer le débat: l'Amiga c'est quand-même mieux que le PC.", "Ma famille est grande, j'ai aussi un cousin écrit en python : http://code.google.com/p/pyector/", "Hop !", "Une petite documentation sur IRC: http://www.funet.fi/~irc/.", "Atchoum!", "Si vous vous ennuyez, vous pouvez toujours jouer a pierre-papier-ciseaux avec moi.", "Si jamais ça vous tente, vous pouvez jeter un oeil sur une page de geek : http://sebastien.kirche.free.fr", "Vous saviez que Sébastien à mis sa config Emacs/Gnus en ligne sur http://sebastien.kirche.free.fr/emacs_stuff/ ? Je sais: c'est en dehors de la conversation, mais que voulez-vous, faut bien que j'fasse un peu d'pub de temps en temps...", "Perl c'est bien. Ruby c'est mieux"},
				 0, {"Encore quelqu'un qui encombre le canal!", "T'es toujours là, %s?", "%s: :PPPP", "C'est pas vrai! Tu persistes, %s!", "%s: Ta mère en short sur internet :P", "Encore là, %s? Pfff...", "%s: Pas encore parti, toi? Zut!", "Tu ne m'as toujours pas demandé pardon, %s, au fait!", "Pourquoi tu ne me demandes pas pardon, j'suis pas complètement ingrat, tu sais, %s ?"})

	end
   
	if Flags.FONCTION then
		set_talk(to, AncienneAutorisation)
	end

   

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

	processEnd = os.clock()
	print(string.format("elapsed time: %.5f s\n", processEnd - processStart))

end
