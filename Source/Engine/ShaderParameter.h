#pragma once

namespace k3d
{
	enum class ShaderParamType
	{
		Vector,
		Matrix,
		MatrixArray,
		ShaderResource,
		UnOrderedAccess,
		ConstantBuffer,
		Sampler,
		Entity
	};

	class ShaderParameter
	{
	public:
		ShaderParameter();
		virtual ~ShaderParameter();

		void SetName(const char * paramName);
		std::string & GetName();

		virtual const ShaderParamType GetParamType() = 0;

		void InitParamData(void* pData);

		virtual void SetParamData(void* pData, uint32 threadID = 0) = 0;

		uint32 GetValueID(uint32 threadID = 0);

	protected:

		std::string m_ParameterName;
		uint32		m_ValueID[4];
	};

	class ShaderParameterManager
	{
	public:
		ShaderParameterManager();
		virtual ~ShaderParameterManager();
	};

}