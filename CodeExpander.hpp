#pragma once

namespace CodeExpander
{

	struct Code
	{
		Array<String> includedFiles;
		Array<String> libraries;
		Array<String> codes;
		Array<String> controllers;

		void write(String fileName)
		{
			TextWriter writer{ fileName };
			if (not writer)
			{
				throw Error{ U"Failed to open {} to write."_fmt(fileName)};
			}

			for (auto controller : controllers)
			{
				writer << controller;
			}

			for (auto lib : libraries)
			{
				writer << U"# include <{}>"_fmt(lib);
			}
			for (auto code : codes)
			{
				writer << code;
			}
		}
	};
	
	String getInclude(String line)
	{
		if (line.includes('"'))
		{
			return line.split('"')[1];
		}
		else
		{
			return line.split('<')[1].split('>')[0];
		}
	}

	void AnalyzeCode(String fileName, Code& res)
	{
		fileName = FileSystem::FullPath(fileName);
		if (res.includedFiles.contains(fileName)) return;
		res.includedFiles << fileName;
		TextReader reader{ fileName };
		if (not reader) throw Error(U"Failed to read source file: {}"_fmt(fileName));

		String line;
		const String baseName = FileSystem::BaseName(fileName);
		Array<String> bufferedSourceFiles;
		bool siv3dIgnore = false;

		while (reader.readLine(line))
		{
			if (siv3dIgnore)
			{
				if (line.starts_with(U"#endif")) siv3dIgnore = false;
				continue;
			}

			if (not line.starts_with('#'))
			{
				res.codes << line;
				continue;
			}
			if (line.includes(U"pragma once")) continue;

			if (line.includes(U"include"))
			{
				String includedFile = getInclude(line);
				String path = FileSystem::FullPath(FileSystem::PathAppend(FileSystem::ParentPath(fileName), includedFile));
				
				if (line.includes('<'))
				{
					res.libraries << includedFile;
				}
				else if (baseName != FileSystem::BaseName(includedFile))
				{
					AnalyzeCode(path, res);
					if (path.ends_with(U".hpp"))
					{
						if (FileSystem::Exists(path.replaced(U".hpp", U".cpp")))
						{
							bufferedSourceFiles << path.replaced(U".hpp", U".cpp");
						}
					}
				}
				continue;
			}

			if (line.includes(U"ifdef SIV3D_INCLUDED"))
			{
				siv3dIgnore = true;
				continue;
			}

			res.controllers << line;
		}

		for (auto sourceFile : bufferedSourceFiles)
		{
			AnalyzeCode(sourceFile, res);
		}

		res.libraries.stable_unique();
	}

	void Expand(const String& targetFile)
	{
		Code code;

		AnalyzeCode(targetFile, code);

		String outFile = FileSystem::ParentPath(targetFile) + FileSystem::FileName(targetFile) + U".expanded." + FileSystem::Extension(targetFile);

		code.write(outFile);
	}
};
