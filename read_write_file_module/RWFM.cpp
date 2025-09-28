#include "RWFM.hpp"

const RWFM::StatusCodes& RWFM::getStatusCode() const noexcept
{
	return m_code;
}

RWFM::RWFM() : m_code(StatusCodes::STATUS_CONSTRUCTED), m_filename("")
{
	m_buffer.reserve(1024);
}

RWFM::RWFM(std::string_view filename) : m_code(StatusCodes::STATUS_CONSTRUCTED), m_filename(filename)
{
	m_buffer.reserve(1024);
}

const std::vector<std::string>& RWFM::getBuffer() const noexcept
{
	return m_buffer;
}

std::string_view RWFM::getFileName() const noexcept
{
	return m_filename;
}

void RWFM::setFileName(const std::string& new_filename) noexcept
{
	if (new_filename == m_filename)
		return;

	m_filename = new_filename;
}

RWFM::StatusCodes RWFM::ReadFile()
{
	m_buffer.clear();
	std::ifstream readF(m_filename);
	if (!readF.is_open())
	{
		m_code = StatusCodes::STATUS_ERROR;
		return m_code;
	}

	if (is_empty(readF))
	{
		m_code = StatusCodes::STATUS_EMPTYBUF;
		return m_code;
	}

	std::string input = "";
	while (std::getline(readF, input, '\n'))
	{
		m_buffer.push_back(input);
	}

	m_code = StatusCodes::STATUS_OK;
	return m_code;
}

RWFM::StatusCodes RWFM::WriteToFile(const std::vector<std::string>& data)
{
	std::ofstream writeF(m_filename, std::ios::trunc);
	if (!writeF.is_open())
	{
		m_code = StatusCodes::STATUS_ERROR;
		return m_code;
	}

	for(auto& line : data)
		writeF << line << '\n';

	m_code = StatusCodes::STATUS_OK;
	return m_code;
}
