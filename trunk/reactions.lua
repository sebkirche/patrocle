--traitement des messages

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

   {{"ca va", "ca gaz", "vas-tu", "vas tu", "vas bien", "va bien", "en forme", "le moral", "la forme", "comment va", "comment tu va", "comment te sens-tu", "comment te sens tu", "comment tu te sens", "comment tu t'sens", "ti k'sa va", "ca baigne", "ca boume", "ca roule", "sens bien", "vous allez bien", "allez-vous bien", "comment vous allez", "comment allez-vous", "commen y le" --[[en creole]] , "how are u", "how are you", "how do you do", "Mita kuuluu" --[[en finnois]], "tu rak bikir" --[[en arabe]]},
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

	{{"connard", "couillon", "ducon", "con ", {"con,", "^facon", "^rcon", "^scon", "^conf", "^falcon"}, "gros con", "sale con", "tit con", " des cons", "salaud", {"salop", "^salopette"},"mechant", "injuste", "degueulasse", "malpropre", --[[{"sale", "^salem", "^ssale"]] "tu pues", " puent", "pourceau", "tu sens movais", "tu sens mauvais", "sentent mauvais", "amibe", "amorphe", "leche-cul", "leche cul", "casse-couille", {"pourri", "^pourriez"}, {"tache", "^moustache", "^pistache"},"raclure", "indecent", "vulgaire", "triple buse", "chacal", "primate", "rascal", "minable", "corniaud", "trisomique", "imbecile", "baka" --[[ imbecile en japonais ]],"corniaud", "blaireau", "cafard", "idiot", "stupid", "nigaud", "neuneu", " bete", "faible d'esprit", "attarde mental", "cretin", "debile", "niais", "nul ", {" nul", "^match"},"abruti", "ahuri", "gogol", "naze", "pignouf", "mauvais", "mongole", "andouille", "cornichon", "banane", "quetsche", "cake", "enfoire", "encule", "enqule" --[[ Y'en a qui sont nuls en orthographe! ]],{"fuck","^!fuck"}, "va te faire", "te faire foutre", "lopete", {"lopette", "^salopette"},"tapette", {"pd", "^pdt", "^repdel"},"tantouse", "impuissant", "putain", "putin" --[[ faute d'ortho ]],"bitch", "tu suces", "tete de noeud", "fumier", "trou du c", "trou duc", "trouduc", "chiant", {"fais chier", "^te fais chier"},	{"fait chier", "^se fait chier"},"vas chier", " de daube", "charogne", "lourd", {"merd", "^demerd"},"espece d", "ta gueule", "ta geule" --[[ Faute d'orthographe courante ]], "face de ", "menteur", "malhonnete", "escroc", "charlatan", "canaille", "chiendent", {"patate", "patater"}, {"outre", "^foutre"},"bouffon", --[["pelo") ?!? ]]"pov gars", "moche", "affreu", "laid", "vilain", "morveux", "alcolo", "alcoolo", "alcoolique", "alcolique", "navrant", "lamentable", "faineant", "faignant", "flemmard", "bouzeu", "bouseu", "sagouin", "sagoin"},
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

	{{"calme", "mort", {" quiet", "^inquiet"}, "un ange passe", "tranquille", "reposant"},
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
	 "JEMENVAIS"},

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

	{{{"ami", "^gamin"}, "copain", "camarade", "pote", "poto"},
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
	 "GROS_MOT"},

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

--[[
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

   ProcessReponsesSimples(from, to)

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


	if string.find(from, "seki@falbala.seki.fr") then
	   if string.find(message,"phrasescount ?") == 1 then 
		  repondre(from, to, 0, {"sur " .. to .. ", " .. nbphrases .. " traitees"}, 0, {})
	   end
	   if string.find(message,"luaversion ?") == 1 then
		  repondre(from, to, 0, {"je fonctionne avec " .. _VERSION}, 0, {})
	   end
	end
	if string.find(from, "!n=karim@") then
	   repondre(from, to, 0, {"mon copain kh", "tu me rappelles quelqu'un, kh"}, 0, {})
	end
	
	--repondre(from, to, 0, {"fin d'analyse"}, 0, {})


end
