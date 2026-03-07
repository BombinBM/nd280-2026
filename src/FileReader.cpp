#include "FileReader.h"
#include <iostream>
#include <TBranch.h>
#include <TList.h>

FileReader::FileReader(const std::string& filename, const std::string& treename) : treeName(treename)
{
    OpenFile(filename, treename);
}

bool FileReader::OpenFile(const std::string& filename, const std::string& treename)
{
    CloseFile();

    rootFile = std::make_unique<TFile>(filename.c_str(), "READ");
    if (!rootFile || rootFile->IsZombie()) 
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return false;
    }
    this->treeName = treename;
    InitializeReader();

    return true;
}

void FileReader::CloseFile()
{
    branchReaders.clear();
    treeReader.reset();
    rootFile.reset();
}

void FileReader::InitializeReader()
{
    if (!rootFile)
    {return;}

    auto* tree = dynamic_cast<TTree*>(rootFile->Get(treeName.c_str()));
    if (!tree)
    {
        std::cerr << "Error: Tree " << treeName << " not found in file" << std::endl;
        return;
    }

    treeReader = std::make_unique<TTreeReader>(tree);
}

bool FileReader::GetEntry(long long entry)
{
    if (!treeReader || !treeReader->GetTree())
    {return false;}

    auto bytesRead = treeReader->GetTree()->GetEntry(entry);
    return bytesRead > 0;
}

long long FileReader::GetEntries()
{
    if(!treeReader || !treeReader->GetTree())
    {return -1;}
    return treeReader->GetTree()->GetEntries();
}

bool FileReader::Next()
{
    return treeReader && treeReader->Next();
}

void FileReader::PrintBranches()
{
    if(!treeReader || !treeReader->GetTree())
    {
        std::cerr << "No tree open" << std::endl;
        return;
    }
    auto* branches = treeReader->GetTree()->GetListOfBranches();
    
    if(!branches)
    {return;}

    std::cout << "Branches in tree " << treeName << " :" << std::endl;
    for(int i=0; i<branches->GetEntries(); i++)
    {
        auto* branch = dynamic_cast<TBranch*>(branches->At(i));
        if(branch)
        {
            std::cout << "\t" << i << ": " << branch->GetName() << "type: " << branch->GetClassName() << ")" << std::endl;
        }
    }
}

TObjArray* FileReader::GetBranchList()
{
    if(!treeReader || !treeReader->GetTree())
    {return nullptr;}

    return treeReader->GetTree()->GetListOfBranches();
}