#ifndef RWFM_HPP
#define RWFM_HPP

#include<fstream>
#include<string>
#include<vector>
#include<string_view>

class RWFM // RWFC - read or write to file Module
{
private:
	std::string m_path;
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
	RWFM(std::string_view path);
	RWFM(std::string&& path);
	~RWFM();

	const std::vector<std::string>& getBuffer() const noexcept;
	std::vector<std::string>& getBuffer() noexcept;

	std::string_view getFileName() const noexcept;

	void setFileName(const std::string& new_path) noexcept;
	void setFileName(std::string&& new_path) noexcept;

	StatusCodes ReadFile();

	StatusCodes WriteToFile(const std::vector<std::string>& data);

private:
	StatusCodes m_code;
};

#endif
