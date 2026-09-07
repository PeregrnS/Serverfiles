#include "stdafx.h"
#include "locale_service.h"
#ifdef ENABLE_NEW_LOCALE_STRING_READING
#include <fstream>
#include <stdlib.h>
#endif

typedef std::map< std::string, std::string > LocaleStringMapType;

LocaleStringMapType localeString;

int g_iUseLocale = 0;

void locale_add(const char **strings)
{
	LocaleStringMapType::const_iterator iter = localeString.find( strings[0] );

	if( iter == localeString.end() )
	{
		localeString.emplace(strings[0], strings[1]);
	}
}

const char * locale_find(const char *string)
{
	if (0 == g_iUseLocale || LC_IsKorea() || LC_IsWE_Korea())
	{
		return (string);
	}

	LocaleStringMapType::const_iterator iter = localeString.find( string );

	if( iter == localeString.end() )
	{
#if defined(__BL_CLIENT_LOCALE_STRING__)
		static char s_line[1024] = "";
		strlcpy(s_line, string, sizeof(s_line));
#else
		static char s_line[1024] = "@0949";
		strlcpy(s_line + 5, string, sizeof(s_line) - 5);
#endif

		sys_err("LOCALE_ERROR: \"%s\";", string);
		return s_line;
	}

	return iter->second.c_str();
}

const char *quote_find_end(const char *string)
{
	const char  *tmp = string;
	int         quote = 0;

	while (*tmp)
	{
		if (quote && *tmp == '\\' && *(tmp + 1))
		{
			switch (*(tmp + 1))
			{
				case '"':
					tmp += 2;
					continue;
			}
		}
		else if (*tmp == '"')
		{
			quote = !quote;
		}
		else if (!quote && *tmp == ';')
			return (tmp);

		tmp++;
	}

	return (NULL);
}

char *locale_convert(const char *src, int len)
{
	const char	*tmp;
	int		i, j;
	char	*buf, *dest;
	int		start = 0;
	char	last_char = 0;

	if (!len)
		return NULL;

	buf = M2_NEW char[len + 1];

	for (j = i = 0, tmp = src, dest = buf; i < len; i++, tmp++)
	{
		if (*tmp == '"')
		{
			if (last_char != '\\')
				start = !start;
			else
				goto ENCODE;
		}
		else if (*tmp == ';')
		{
			if (last_char != '\\' && !start)
				break;
			else
				goto ENCODE;
		}
		else if (start)
		{
ENCODE:
			if (*tmp == '\\' && *(tmp + 1) == 'n')
			{
				*(dest++) = '\n';
				tmp++;
				last_char = '\n';
			}
			else
			{
				*(dest++) = *tmp;
				last_char = *tmp;
			}

			j++;
		}
	}

	if (!j)
	{
		M2_DELETE_ARRAY(buf);
		return NULL;
	}

	*dest = '\0';
	return (buf);
}

#define NUM_LOCALES 2

#ifdef ENABLE_NEW_LOCALE_STRING_READING
void locale_init(const char *filename)
{

	auto printError = [] (const char* fmt, ...) -> void {
		char szBuffer[400] = {};

		va_list args;

		va_start(args, fmt);
			vsnprintf(szBuffer, sizeof(szBuffer), fmt, args);
		va_end(args);

		fprintf(stderr, "%s\n", szBuffer);
	};

	auto isOutString = [](size_t pos) -> bool {
		return pos==std::string::npos;
	};

	auto isEmptyString = [&isOutString](const std::string& line) ->bool {
		return isOutString(line.find_first_not_of(" \t\r\n"));
	};

	auto getToken = [&isOutString](std::string& line) -> std::string {
		size_t first    = line.find("\"");
		size_t last        = line.find_last_of("\"");

		if( first == last || isOutString(first) || isOutString(last) || first == line.length()-1)
			return "";

		first++;
		return line.substr(first, (last-first));
	};

	//initialize two empty container strings
	std::string header="",line="";

	std::ifstream localestringfile(filename);
	if (!localestringfile.is_open())
	{
		printError("CANNOT OPEN LOCALE_STRING FILE! [%s] -ERROR",filename);
		return;
	}

	int lineIndex =0;
	while (std::getline(localestringfile, line))
	{
		lineIndex++;
		size_t commentIndex = line.find("///");

		if(!isOutString(commentIndex))
			line = line.substr(0,commentIndex);

		if(isEmptyString(line))
			continue;

		std::string token = getToken(line);
		if (isEmptyString(token))
		{
			printError("LOCALE STRING WRONG SYNTAX AT LINE %d - ERROR ", lineIndex);
			return;
		}

		if(header.empty())
			header = token;

		else
		{
			if(localeString.find(header) != localeString.end())
				printError("LOCALE STRING - DOUBLE HEADER FOUND. (header [%s] , line index %d) - WARNING", header.c_str() , lineIndex);

			localeString[header] = token;
			header = "";
		}
	}

	if(!header.empty())
		printError("LOCALE STRING : !HEADER.EMPTY (bad reading) -ERROR");
	else
		printError("LOCALE STRING : Loaded %u elements in %d lines.",localeString.size() , lineIndex);

}
#else
void locale_init(const char *filename)
{
	FILE        *fp = fopen(filename, "rb");
	char        *buf;

	if (!fp) return;

	fseek(fp, 0L, SEEK_END);
	int i = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	i++;

	buf = M2_NEW char[i];

	memset(buf, 0, i);

	fread(buf, i - 1, sizeof(char), fp);

	fclose(fp);

	const char * tmp;
	const char * end;

	char *	strings[NUM_LOCALES];

	if (!buf)
	{
		sys_err("locale_read: no file %s", filename);
		exit(1);
	}

	tmp = buf;

	do
	{
		for (i = 0; i < NUM_LOCALES; i++)
			strings[i] = NULL;

		if (*tmp == '"')
		{
			for (i = 0; i < NUM_LOCALES; i++)
			{
				if (!(end = quote_find_end(tmp)))
					break;

				strings[i] = locale_convert(tmp, end - tmp);
				tmp = ++end;

				while (*tmp == '\n' || *tmp == '\r' || *tmp == ' ') tmp++;

				if (i + 1 == NUM_LOCALES)
					break;

				if (*tmp != '"')
				{
					sys_err("locale_init: invalid format filename %s", filename);
					break;
				}
			}

			if (strings[0] == NULL || strings[1] == NULL)
				break;

			locale_add((const char**)strings);

			for (i = 0; i < NUM_LOCALES; i++)
				if (strings[i])
					M2_DELETE_ARRAY(strings[i]);
		}
		else
		{
			tmp = strchr(tmp, '\n');

			if (tmp)
				tmp++;
		}
	}
	while (tmp && *tmp);

	M2_DELETE_ARRAY(buf);
}
#endif

//martysama0134's aad276684955eb3421d3edd3e79cd0dc
