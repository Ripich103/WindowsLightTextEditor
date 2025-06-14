#ifndef RWFM_H
#define RWFM_H

#include<fstream>
#include<string>
#include<vector>

class RWFM // RWFC - Read and Write to File Module
{
private:
	inline enum class StatusCodes : short {
		STATUS_OK = 1,
		STATUS_ERROR = -1,
		STATUS_CONSTRUCTED = 0
	};

	std::string m_filename;
	std::vector<std::string> m_buffer;

	StatusCodes m_code;

public:
	const StatusCodes& getStatusCode() const noexcept;

	explicit RWFM();
	RWFM(const std::string& filename);

	const std::vector<std::string>& getBuffer() const noexcept;

	const std::string& getFileName() const noexcept;

	void setFileName(const std::string& new_filename) noexcept;

	StatusCodes ReadFile();

	StatusCodes WriteToFile(const std::vector<std::string>& data);
};

#endif
