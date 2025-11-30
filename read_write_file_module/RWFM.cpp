#include "RWFM.hpp"

const RWFM::StatusCodes& RWFM::getStatusCode() const noexcept
{
	return m_code;
}

RWFM::RWFM() : m_code(StatusCodes::STATUS_CONSTRUCTED), m_path(L"")
{
	m_buffer.reserve(1024);
}

RWFM::RWFM(const std::wstring& path) : m_code(StatusCodes::STATUS_CONSTRUCTED), m_path(path)
{
	m_buffer.reserve(1024);
}

RWFM::RWFM(std::wstring&& path) : m_code(StatusCodes::STATUS_CONSTRUCTED), m_path(std::move(path))
{
	m_buffer.reserve(1024);
}

RWFM::~RWFM()
{
	m_buffer.clear();
	m_path.clear();
	m_code = StatusCodes::STATUS_EMPTYBUF;
}

const std::vector<std::wstring>& RWFM::getBuffer() const noexcept
{
	return m_buffer;
}

std::vector<std::wstring>& RWFM::getBuffer() noexcept
{
	return m_buffer;
}

std::wstring RWFM::getFileName() const noexcept
{
	return m_path;
}

void RWFM::setFileName(const std::wstring& new_path) noexcept
{
	if (new_path == m_path)
		return;

	m_path = new_path;
}

void RWFM::setFileName(std::wstring&& new_path) noexcept
{
	if (new_path == m_path)
		return;

	m_path = std::move(new_path);
}

RWFM::StatusCodes RWFM::ReadFile()
{
	m_buffer.clear();
	std::wifstream readF(m_path);

	if (!readF.is_open())
	{
		m_code = StatusCodes::STATUS_ERROR;
		return m_code;
	}

	if (is_empty(readF))
	{
		m_code = StatusCodes::STATUS_EMPTYBUF;
		readF.close();
		return m_code;
	}

	std::wstring input = L"";
	while (std::getline(readF, input, L'\n'))
	{
		m_buffer.push_back(input);
	}

	m_code = StatusCodes::STATUS_OK;
	readF.close();
	return m_code;
}

RWFM::StatusCodes RWFM::WriteToFile(const std::vector<std::wstring>& data)
{
	std::wofstream writeF(m_path, std::ios::trunc);
	if (!writeF.is_open())
	{
		m_code = StatusCodes::STATUS_ERROR;
		return m_code;
	}

	for(auto& line : data)
		writeF << line.c_str() << '\n';

	m_code = StatusCodes::STATUS_OK;
	writeF.close();
	return m_code;
}
