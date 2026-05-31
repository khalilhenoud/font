#if !defined(FONT_API)
	#define FONT_API /* NOTHING */

	#if defined(WIN32) || defined(WIN64)
		#undef FONT_API
		#if defined(font_EXPORTS)
			#define FONT_API __declspec(dllexport)
		#else
			#define FONT_API __declspec(dllimport)
		#endif
	#endif // defined(WIN32) || defined(WIN64)

#endif // !defined(FONT_API)

