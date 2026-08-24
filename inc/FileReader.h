#ifndef FILE_READER_H
#define FILE_READER_H

#include <iostream>

#include <string>
#include <memory>
#include <unordered_map>
#include <any>
#include <typeindex>
#include <stdexcept>

#include "TChain.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TTree.h"

class FileReader
{
public:

    FileReader() = default;
    FileReader(const std::string& filename, const std::string& treename);
    // Запрещаем копирование для того, чтоб не пересекались индексы
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;

    // Разрешаем перемещение от объекта к объекту
    FileReader(FileReader&&) = default;
    FileReader& operator=(FileReader&&) = default;

    bool OpenFile(const std::string& filename, const std::string& treename);
    void CloseFile();
    bool IsOpen(){return rootFile!= nullptr && !rootFile->IsZombie();}


    template<typename T>
    bool SetBranchAddres(const std::string& branchname, T* address);

    template<typename T>
    bool AddBranchReader(const std::string& branchname);
 
    template<typename T>
    T* GetBranchReader(const std::string& branchname);

    bool GetEntry(long long entry);
    long long GetEntries();
    bool Next();

    template<typename T>
    T GetValue(const std::string& branchname);

    template<typename T>
    std::vector<T> GetArray(const std::string& branchname);

    void PrintBranches();
    TObjArray* GetBranchList();

private:

    void InitializeReader();

    std::unique_ptr<TChain> chain;
    std::unique_ptr<TFile> rootFile;
    std::unique_ptr<TTreeReader> treeReader;
    std::string treeName; 

    std::unordered_map<std::string, std::any> branchReaders;
};

template<typename T>
bool FileReader::SetBranchAddres(const std::string& branchname, T* addres)
{

    if(!chain) return false;
    return chain->SetBranchAddress(branchname.c_str(), addres) >= 0;

    // if(!treeReader || !treeReader->GetTree())
    // {
    //     return false;
    // }

    // auto* tree = treeReader->GetTree();
    // auto* branch = tree->GetBranch(branchname.c_str());

    // if(!branch)
    // {return false;}
    // branch->SetAddress(addres);
    // return true;
}

template<typename T>
bool FileReader::AddBranchReader(const std::string& branchname)
{
    if(!treeReader)
    {return false;}

    try
    {
        if constexpr (std::is_same_v<T, std::vector<typename T::value_type>>)
        {
            using ElementType = typename T::value_type;
            auto reader = std::make_unique<TTreeReaderArray<ElementType>>(*treeReader, branchname.c_str());
            branchReaders[branchname] = std::move(reader);
        }
        else
        {
            auto reader = std::make_unique<TTreeReaderValue<T>>(*treeReader, branchname.c_str());
            branchReaders[branchname] = std::move(reader);
        }
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

template<typename T>
T* FileReader::GetBranchReader(const std::string& branchname)
{
    auto it = branchReaders.find(branchname);
    if(it == branchReaders.end())
    {return nullptr;}

    try
    {
        if constexpr(std::is_same_v<T, std::vector<typename T::value_type>>)
        {
            using ElementType = typename T::value_type;
            return std::any_cast<std::unique_ptr<TTreeReaderValue<T>>>(&it->second)->get();
        }
    }
    catch(const std::bad_any_cast& e)
    {
        return nullptr;
    }
    
}

template<typename T>
T FileReader::GetValue(const std::string& branchname)
{
    auto it = branchReaders.find(branchname);
    if (it == branchReaders.end())
    {
        throw std::runtime_error("Branch reader not found: " + branchname);
    }

    try
    {
        auto& readerPtr = std::any_cast<const std::unique_ptr<TTreeReaderValue<T>>&>(it->second);
        if (readerPtr && readerPtr->IsValid())
            return **readerPtr;
        // auto* reader = std::any_cast<std::unique_ptr<TTreeReaderValue<T>>>(&it->second);
        // if (reader && *reader)
        // {return **(*reader);}

    } catch (const std::bad_any_cast& e)
    {
        throw std::runtime_error("Type mismatch for branch" + branchname);
    }
    throw std::runtime_error("Failed to get value for branch: " + branchname);
}

template<typename T>
std::vector<T> FileReader::GetArray(const std::string& branchname)
{
    auto it = branchReaders.find(branchname);
    if (it == branchReaders.end())
    {
        throw std::runtime_error("Branch reader not found: " + branchname);
    }
    
    try
    {
        auto& readerPtr = std::any_cast<const std::unique_ptr<TTreeReaderArray<T>>&>(it->second);
        if (readerPtr)
        {
            std::vector<T> result;
            result.reserve(readerPtr->GetSize());
            for (const auto& element : *readerPtr)
            {
                result.push_back(element);
            }
            return result;
        }
    }
    catch(const std::bad_any_cast& e)
    {
        throw std::runtime_error("Type mismatch for branch: " +branchname);
    }
    throw std::runtime_error("Failed to get array for branch: "+branchname);

    
    // auto it = branchReaders.find(branchname);
    // if (it == branchReaders.end())
    // {
    //     throw std::runtime_error("Branch reader not found: " +branchname);
    // }

    // try
    // {
    //     auto* reader = std::any_cast<std::unique_ptr<TTreeReaderArray<T>>>(&it->second);
    //     if (reader && *reader)
    //     {
    //         std::vector<T> result;
    //         result.reserve((*reader)->GetSize);
    //         for (const auto& element: **reader)
    //         {
    //             result.push_back(element);
    //         }
    //         return result;
    //     }
    // }
    // catch(const std::bad_any_cast& e)
    // {
    //     throw std::runtime_error("Type mismatch for branch: " +branchname);
    // }
    // throw std::runtime_error("Failed to get array for branch: "+branchname);
}


#endif