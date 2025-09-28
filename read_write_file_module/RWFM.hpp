#ifndef RWFM_HPP
#define RWFM_HPP

#include<fstream>
#include<string>
#include<vector>
#include<string_view>

class RWFM // RWFC - read or write to file Module
{
private:
	std::string m_filename;
	std::vector<std::string> m_buffer;

	static inline bool is_empty(std::ifstream& pFile)
	{
		return pFile.peek() == std::ifstream::traits_type::eof();
	}

public:

	enum class StatusCodes : short {
		STATUS_OK = 1,
		STATUS_EMPTYBUF = 2,
		STATUS_ERROR = -1,
		STATUS_CONSTRUCTED = 0
	};

	const StatusCodes& getStatusCode() const noexcept;

	explicit RWFM();
	RWFM(std::string_view filename);

	const std::vector<std::string>& getBuffer() const noexcept;

	std::string_view getFileName() const noexcept;

	void setFileName(const std::string& new_filename) noexcept;

	StatusCodes ReadFile();

	StatusCodes WriteToFile(const std::vector<std::string>& data);

private:
	StatusCodes m_code;
};

#endif
