/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int snap      = 32; 
static const int swallowfloating    = 0;        /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 5;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Hack:size=12" };
static const char dmenufont[]       = "Hack:size=12";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_orange[] 		= "#FE8019";
static const char col_mixed_green[] = "#8EC07C";
static const char col_gruvbg[] 		= "#282828";
static const char col_gruvfont[] 	= "#FBF1C7";
static const char col_gruvfontdark[]= "#A89984";
static const char col_gruvblue[]	= "#458588";
// static const char *colors[][3]      = {
// 	/*               fg         bg         border   */
// 	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
// 	[SchemeSel]  = { col_gray4, col_mixed_green,  col_mixed_green  },
// 	[SchemeStatus]  = { col_gruvfont, col_gruvbg,  "#000000"  }, // Statusbar right {text,background,not used but cannot be empty}
// 	[SchemeTagsSel]  = { col_gruvfont, col_gruvblue,  "#000000"  }, // Tagbar left selected {text,background,not used but cannot be empty}
// 	[SchemeTagsNorm]  = { col_gruvfontdark, col_gruvbg,  "#000000"  }, // Tagbar left unselected {text,background,not used but cannot be empty}
// 	[SchemeInfoSel]  = { col_gruvfont, col_gruvblue,  "#000000"  }, // infobar middle  selected {text,background,not used but cannot be empty}
// 	[SchemeInfoNorm]  = { col_gruvfontdark, col_gruvbg,  "#000000"  }, // infobar middle  unselected {text,background,not used but cannot be empty}
// };
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};
static const char *const autostart[] = {
	//"xrandr", "--output", "eDP-1-1", "--primary", "--output", "HDMI-0", "--auto", "--left-of", "eDP-1-1", NULL,
	"xrandr", "--output", "HDMI-0", "--mode", "1920x1080", "--right-of", "DP-0", "--rotate", "right", "--output", "DP-0", "--mode", "1920x1080", "--rate", "143.85", "--primary", NULL,
	"slstatus", NULL,
	// "easyeffects", "--gapplication-service", NULL,
	// "feh", "--bg-fill", "~/Pictures/.bg/*", NULL, // different monitor will assigned different picture as listed in folder
	NULL
};

/* tagging */
static const char *tags[] = { "", "", "", "", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class		instance		title           tags mask 	isfloating	isterminal 	noswallow 	monitor */
	{ "firefox",	NULL,       	NULL,   		1 << 1,     0,          0, 			1, 			-1 	},
	{ "Steam",	 	"Steam", 		NULL, 			1 << 3,		1,			0, 			1, 			0 	},
	{ "dota2", 		"dota2", 		NULL,			1 << 8, 	0,			0, 			1, 			0 	},
	{ "steam_app_", "steam_app_", 	NULL, 			1 << 7, 	1,			0, 			1, 			0 	},
	{ "hl_", 		NULL 			,NULL,   		1 << 7,     1,          0, 			1, 			0 	},
		
	// swallow config, there are many stuffs are automatically like `chromium google.com`
	{ NULL,      	NULL,     		"Event Tester", 0,         	0,          0,          1,        	-1 	}, /* xev */
	{ "st-256color",NULL, 			NULL, 			0, 			0, 			1, 			1, 			-1	},
	{ "mpv", 		NULL, 			NULL, 			0, 			0, 			0, 			0,			-1 	},
	{ "libreoffice", NULL,			NULL,			0,			0,			0,			0,			-1	},
	{ "Pcmanfm",  	NULL,      		NULL,   		0,       	1,          0, 			0, 			-1 	},
	{ "discord", 	NULL, 			NULL, 			1 << 3, 	1, 			0, 			0,			1 	},
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const Bool statusall   = True;

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/usr/local/bin/st", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *brightness_up[]  =   { "light", "-A", "5", NULL };
static const char *brightness_down[]  = { "light", "-U", "5", NULL };
static const char *volume_up[] = {"amixer", "-q", "sset", "Master", "2%+", NULL};
static const char *volume_down[] = {"amixer", "-q", "sset", "Master", "2%-", NULL};
static const char *volume_mute[] = {"amixer", "-q", "sset", "Master", "toggle", NULL};
static const char *flameshot_gui[] = {"flameshot", "gui", NULL};
static const char *flameshot_clipboard[] = {"flameshot", "full", "-c", NULL};
static const char *flameshot_launcher[] = {"flameshot", "launcher", NULL};
static const char *spectacle[] = {"spectacle", NULL};
static const char *slock[] = {"slock", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_x,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_h,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_quotedbl,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_v,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_z,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY|ShiftMask,           	XK_t, 	   cyclelayout,    {.i = +1 } },
	{ MODKEY,             			XK_space,  togglefloating, 	{0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglealwaysontop, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ 0,       		XF86XK_MonBrightnessUp, 	spawn,    	{.v = brightness_up}},
	{ 0,       		XF86XK_MonBrightnessDown, 	spawn, 		{.v = brightness_down}},
	{ 0, 			XF86XK_AudioRaiseVolume, 	spawn, 		{.v = volume_up}},
	{ 0, 			XF86XK_AudioLowerVolume, 	spawn, 		{.v = volume_down}},
	{ 0, 			XF86XK_AudioMute, 			spawn, 		{.v = volume_mute}},
	// { 0, 							XK_Print, 	spawn,		{.v = flameshot_gui}},
	// { MODKEY, 						XK_Print, 	spawn,		{.v = flameshot_clipboard}},
	// { MODKEY|ShiftMask, 			XK_Print, 	spawn,		{.v = flameshot_launcher}},
	{ 0, 							XK_Print, 	spawn,		{.v = flameshot_gui}},
	{ MODKEY|ShiftMask, 			XK_l, 		spawn,		{.v = slock}},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask|ControlMask, XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        cyclelayout,    {.i = +1 } },
	{ ClkLtSymbol,          0,              Button3,        cyclelayout,    {.i = -1 } },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,   
         {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
