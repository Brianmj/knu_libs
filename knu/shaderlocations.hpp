#ifndef KNU_SHADER_LOCATIONS
#define KNU_SHADER_LOCATIONS

namespace knu
{
	enum class AttributeLocations
	{
		posAttrib,
		texcAttrib,
		normAttrib,
        colAttrib,
        instAttrib      // instance attribute
	};

	enum class UniformLocations
	{
		mvMatrixUniform,
		texUniform = 3,
		projMatrixUniform = 4,

	};
}

#endif