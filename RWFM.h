#ifndef RWFM_H
#define RWFM_H

#include<fstream>
#include<string>
#include<vector>

class RWFM // RWFC - Read and Write to File Module
{
private:
	std::string m_filename;
	std::vector<std::string> m_buffer;

	inline bool is_empty(std::ifstream& pFile)
	{
		return pFile.peek() == std::ifstream::traits_type::eof();
	}

public:

	inline enum class StatusCodes : short {
		STATUS_OK = 1,
		STATUS_EMPTYBUF = 2,
		STATUS_ERROR = -1,
		STATUS_CONSTRUCTED = 0
	};

	const StatusCodes& getStatusCode() const noexcept;

	explicit RWFM();
	RWFM(const std::string& filename);

	const std::vector<std::string>& getBuffer() const noexcept;

	const std::string& getFileName() const noexcept;

	void setFileName(const std::string& new_filename) noexcept;

	StatusCodes ReadFile();

	StatusCodes WriteToFile(const std::vector<std::string>& data);

private:
	StatusCodes m_code;
};

#endif
