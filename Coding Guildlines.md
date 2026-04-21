# Coding Guidlines for DeepSkyStacker 
## Casting
* Avoid C casts, prefer C++ casts (static_cast, const_cast, reinterpret_cast)

    Rationale: Both reinterpret_cast and C-style casts are dangerous, but at least reinterpret_cast won't remove the const modifier
* Don't use dynamic_cast for QObject subclasses, use qobject_cast for QObjects or refactor your design, for example by introducing a type() method (see QListWidgetItem)
* Consider using a constructor to cast simple types: int(myFloat) instead of (int)myFloat.
## Naming of variables and member functions.
* Hungarian notation is anathema as far as I am concerned.   If you must name class member variables m_variableName then I will accept that practice, though I prefer the use of a trailing underscore for that purpose.  For example:

		std::unique_ptr<QImage> image_;

* The name of lambdas, functions and variables should be descriptive and use "camel case" for example:

		void setRedoTip(QString string);
		
* Class names should normally start with a Capital letter. You shouldn't add a C to the front of the classname to indicate that the name is that of a class (see Hungarian notation comments above). For example:

		// Correct
		class DeepStack
		{
			:
		}
		// Wrong
		class CDeepStack
		{
			:
		}
		
* Prefer (for example) description() and setDescription() over getDescription() and setDescription()
## Aesthetics
* Strongly prefer enum classes or constinit/constexpr variables over defines.  Old fashioned enums should not be used in new code.
* Prefer using over typedef. If you are working on old code that uses typedefs you should strongly consider changing the code.

		// Deprecated
		typedef std::multiset<triangle> triangleSet;
		// Preferred
		using triangleSet = std::multiset<triangle>;
		
* Braces: The preferred style for DeepSkyStacker is to have the opening brace on the next line:

		// Correct
		if (codec)
		{
		}
		else
		{
		}
		
* Braces: Normally, I suggest that you use braces only when the body of a conditional statement contains more than one line.  Sometimes however, the compiler (particularly clang on macOS) may issue warnings such that you are almost forced to use braces.  The preferred usage is:

		// Preferred
		if (address.empty())
			return false;
		// Preferred
		if (address.empty()) return false;
		
* Line breaks:
	- Comment lines should normally be kept below 80 columns of text.
	- Use blank lines to group statements together where it helps comprehension
	- Always use only one blank line
## Comments
When writing code you should always insert explanatory comments. What was obvious to you on the day that you wrote some code is very unlikely to be so for another developer, or even for you when you need to fix a bug some years later.

Comments should explain the whys and hows:
		
		// Bad
		value++;	// Increment value
		// Good
		//
		// For colour images, the transform function that is applied is:
		// 
		// pixel = (original - offset) x asinh(rgb_original x beta)
		//         ------------------------------------------------
		//                (rgb_original x asinh(beta))
		//
		// rgb_original is computed from the pixel values of the three channels using the rgbblend algorithm
		// rather than using the average of the three  channels.
		//
		// If the average of the three channels were used for rgb_original, one or two channel values will be
		// greater than the average, and can therefore clip.   This can cause colour artefacts when bright,
		// strongly coloured regions of the image are stretched.   To mitigate this problem the RGB blend
		// algorithm was developed by the authors of the GHS Astro tool.
		// 
		// See the source of rgbblend in DSSTools.h for the full details.
		//
		for (i = 0; i < n; i++)

If code is using a "tricky" or "clever" technique, then it's essential that explanatory comments be added.

