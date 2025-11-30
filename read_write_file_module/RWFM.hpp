#ifndef RWFM_HPP
#define RWFM_HPP

#include<fstream>
#include<string>
#include<vector>
#include<string_view>

class RWFM // RWFC - read or write to file Module
{
private:
	std::wstring m_path;
	std::vector<std::wstring> m_buffer;

	static inline bool is_empty(std::wifstream& pFile)
	{
		return pFile.peek() == std::wifstream::traits_type::eof();
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
	RWFM(const std::wstring& path);
	RWFM(std::wstring&& path);
	~RWFM();

	const std::vector<std::wstring>& getBuffer() const noexcept;
	std::vector<std::wstring>& getBuffer() noexcept;

	std::wstring getFileName() const noexcept;

	void setFileName(const std::wstring& new_path) noexcept;
	void setFileName(std::wstring&& new_path) noexcept;

	StatusCodes ReadFile();

	StatusCodes WriteToFile(const std::vector<std::wstring>& data);

private:
	StatusCodes m_code;
};

#endif
