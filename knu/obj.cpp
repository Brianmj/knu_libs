//
//  obj.cpp
//  Sprite1
//
//  Created by Brian Jones on 3/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#ifdef __APPLE__
//#include <Foundation/Foundation.h>
#endif  // __APPLE__

#include <iostream>
#include <stdexcept>
#include <future>
#include <sstream>
#include <knu/shaderlocations.h>

#include "obj.hpp"

using namespace std;
using namespace knu;


const string MAT_STRING_REP = "(\\.mtl)";
const string OBJ_STRING_REP = "(.obj)";

const string MAT_COUNT = "(Count)";
const string MAT_NAME = "newmtl (\\w+)";
const string MAT_AMBIENT =	"Ka ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";
const string MAT_DIFFUSE =	"Kd ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";
const string MAT_SPECULAR = "Ks ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";
const string MAT_SPECULAR_EXPONENT = "Ns ([-+]?[0-9]*\\.?[0-9]*)";
const string MAT_OPACITY = "d ([-+]?[0-9]*\\.?[0-9]*)";
const string MAT_DIFFUSE_TEXTURE = "map_Kd (\\w+\\.\\w+)";
const string MAT_AMBIENT_TEXTURE = "map_Ka";

const string OBJ_NEW_MESH = "o (\\w+)";
const string OBJ_VERTEX = "v ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";
const string OBJ_TEXTURE = "vt ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";
const string OBJ_NORMAL = "vn ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*) ([-+]?[0-9]*\\.?[0-9]*)";

const string OBJ_USE_MATERIAL = "usemtl (\\w+)";
const string OBJ_FACE_0 = "f (\\d+) (\\d+) (\\d+)";         // 3 indices per line (triangle) | vertex only
const string OBJ_FACE_1 = "f (\\d+) (\\d+) (\\d+) (\\d+)";       // 4 indices per line (Quad) | vertex only
const string OBJ_FACE_2 = "f (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)";    // vertex/texture/normal
const string OBJ_FACE_3 = "f (\\d+)//(\\d+) (\\d+)//(\\d+) (\\d+)//(\\d+)";		// vertex//normal x 3
const string OBJ_FACE_4 = "f (\\d+)/(\\d+) (\\d+)/(\\d+) (\\d+)/(\\d+)";		// vertex/texture


void Obj_Reader::register_handlers()
{
    mat_matches[MAT_COUNT] = &Obj_Reader::mat_count_handler;
    mat_matches[MAT_NAME] = &Obj_Reader::mat_name_handler;
    mat_matches[MAT_AMBIENT] = &Obj_Reader::mat_ambient_handler;
    mat_matches[MAT_DIFFUSE] = &Obj_Reader::mat_diffuse_handler;
    mat_matches[MAT_SPECULAR] = &Obj_Reader::mat_specular_handler;
    mat_matches[MAT_SPECULAR_EXPONENT] = &Obj_Reader::mat_specular_exponent_handler;
    mat_matches[MAT_OPACITY] = &Obj_Reader::mat_opacity_handler;
    mat_matches[MAT_DIFFUSE_TEXTURE] = &Obj_Reader::mat_diffuse_texture_handler;
    
    obj_matches[OBJ_NEW_MESH] = &Obj_Reader::obj_new_mesh_handler;
    obj_matches[OBJ_VERTEX] = &Obj_Reader::obj_vertex_handler;
    obj_matches[OBJ_TEXTURE] = &Obj_Reader::obj_texture_handler;
    obj_matches[OBJ_NORMAL] = &Obj_Reader::obj_normal_handler;
    obj_matches[OBJ_USE_MATERIAL] = &Obj_Reader::obj_add_mat_to_mesh_handler;
    obj_matches[OBJ_FACE_0] = &Obj_Reader::obj_face_handler;
    obj_matches[OBJ_FACE_1] = &Obj_Reader::obj_face_handler;
    obj_matches[OBJ_FACE_2] = &Obj_Reader::obj_face_handler;
	obj_matches[OBJ_FACE_3] = &Obj_Reader::obj_face_handler;
    obj_matches[OBJ_FACE_4] = &Obj_Reader::obj_face_handler;
}

Obj_Reader::Obj_Reader(string material_path, string obj_path)
{
    register_handlers();
    try
    {
        auto mat_future = async(launch::async, &Obj_Reader::read_material_file, this, material_path);
        auto obj_future = async(launch::async, &Obj_Reader::read_obj_file, this, obj_path);
        
        mat_future.get();
        obj_future.get();
    }catch(std::exception &ex)
    {
        std::cerr << ex.what() << std::endl;
        std::exit(-1);
    }
}

std::string Obj_Reader::read_next_line(std::ifstream &file)
{
    string line;
    getline(file, line);
    return line;
}

void Obj_Reader::read_material_file(std::string material_path)
{
    regex r(MAT_STRING_REP);
    smatch match;
    
    if(!regex_search(material_path, match, r))
        throw runtime_error("Not a material file");
    
    ifstream file(material_path);
    
    if(!file)
        throw runtime_error("Could not open material file at " + material_path);
    
    while(!check_eof(file))
    {
        auto current_line = read_next_line(file);
        dispatch_to_mat_parser(current_line);
        
    }
}

void Obj_Reader::read_obj_file(std::string obj_path)
{
    regex r(OBJ_STRING_REP);
    smatch match;
    
    if(!regex_search(obj_path, match, r))
        throw runtime_error("Not a .obj file");
    
    ifstream file(obj_path);
    
    if(!file)
        throw runtime_error("Could not open obj file at" + obj_path);
    
    while(!check_eof(file))
    {
        auto current_line = read_next_line(file);
        dispatch_to_obj_parser(current_line);
    }
}

void Obj_Reader::dispatch_to_mat_parser(std::string line)
{
    auto iterator = find_if(begin(mat_matches), end(mat_matches), Mat_Compare_With_Line(line));
    if(iterator != end(mat_matches))
        (this->*iterator->second)(line);
    
}

void Obj_Reader::dispatch_to_obj_parser(std::string line)
{
    auto iterator = find_if(begin(obj_matches), end(obj_matches), Obj_Compare_With_Line(line));
    if(iterator != end(obj_matches))
        (this->*iterator->second)(line);
}

void Obj_Reader::mat_name_handler(std::string line)
{
    regex r(MAT_NAME);
    smatch match;
    regex_search(line, match, r);
    materials.push_back(Obj_Material());
    materials.back().mat_name = match[1];
}

void Obj_Reader::mat_count_handler(std::string line)
{
    stringstream ss(line);
    string identifier;
    int count = 0;
    ss >> identifier >> identifier >> identifier >> count;
    materials.reserve(count);
    
}

void Obj_Reader::mat_ambient_handler(std::string line)
{
    regex r(MAT_AMBIENT);
    smatch match;
    regex_search(line, match, r);
    
    materials.back().mat_ambient_color = knu::math::Vector4f(stof(match[1]), stof(match[2]), stof(match[3]), 1.0f);
}

void Obj_Reader::mat_diffuse_handler(std::string line)
{
    regex r(MAT_DIFFUSE);
    smatch match;
    regex_search(line, match, r);
    materials.back().mat_diffuse_color = knu::math::Vector4f(stof(match[1]), stof(match[2]), stof(match[3]), 1.0f);
}

void Obj_Reader::mat_specular_handler(std::string line)
{
    regex r(MAT_SPECULAR);
    smatch match;
    regex_search(line, match, r);
    materials.back().mat_specular_color = knu::math::Vector4f(stof(match[1]), stof(match[2]), stof(match[3]), 1.0f);
}

void Obj_Reader::mat_specular_exponent_handler(std::string line)
{
    regex r(MAT_SPECULAR_EXPONENT);
    smatch match;
    regex_search(line, match, r);
    materials.back().mat_specular_exponent = stof(match[1]);
}

void Obj_Reader::mat_opacity_handler(std::string line)
{
    regex r(MAT_OPACITY);
    smatch match;
    
    // Match instead of search in order to avoid conflict with MAT_DIFFUSE (kd #)
    if(regex_match(line, match, r))
    {
        materials.back().mat_opacity = stof(match[1]);
    }
	else	// check if it matches with MAT_DIFFUSE
	{
		
		regex r2(MAT_DIFFUSE);
		if(regex_match(line, match, r2))
		{
			mat_diffuse_handler(line);
		}
	}
}

void Obj_Reader::mat_diffuse_texture_handler(std::string line)
{
    regex r(MAT_DIFFUSE_TEXTURE);
    smatch match;
    auto pos = line.find_last_of("/");
    
    if(line.npos != pos)
    {
        ++pos;
        std::string texture_name(line.begin() + pos, line.end());
        materials.back().mat_diffuse_texture_name = texture_name;
    }else
    {
        regex_search(line, match, r);
        std::string textureName = match[1];
        materials.back().mat_diffuse_texture_name = textureName;
    }
}

void Obj_Reader::mat_ambient_texture_handler(std::string line)
{
    // TODO - THIS FUNCTION NEEDS TO BE TESTED
    regex r(MAT_AMBIENT_TEXTURE);
    smatch match;
    auto pos = line.find_last_of("/");
    
    if(line.npos != pos)
    {
        ++pos;
        std::string texture_name(line.begin() + pos, line.end());
        materials.back().mat_ambient_texture_name = texture_name;
    }else
    {
        regex_search(line, match, r);
        std::string textureName = match[1];
        materials.back().mat_ambient_texture_name = textureName;
    }
}

void Obj_Reader::obj_new_mesh_handler(std::string line)
{
    regex r(OBJ_NEW_MESH);
    smatch match;
    
    regex_search(line, match, r);
    meshes.push_back(Obj_Mesh());
    meshes.back().obj_name = match[1];
    
}

void Obj_Reader::obj_vertex_handler(std::string line)
{
    regex r(OBJ_VERTEX);
    smatch match;
    
    regex_search(line, match, r);
    vertices.push_back(knu::math::Vector3f(stof(match[1]), stof(match[2]), stof(match[3])));
}

void Obj_Reader::obj_texture_handler(std::string line)
{
    regex r(OBJ_TEXTURE);
    smatch match;
    
    regex_search(line, match, r);
    tex_coords.push_back(knu::math::Vector2f(stof(match[1]), stof(match[2])));
}

void Obj_Reader::obj_normal_handler(std::string line)
{
    regex r(OBJ_NORMAL);
    smatch match;
    
    regex_search(line, match, r);
    normals.push_back(knu::math::Vector3f(stof(match[1]), stof(match[2]), stof(match[3])));
}

void Obj_Reader::obj_add_mat_to_mesh_handler(std::string line)
{

    regex r1(OBJ_USE_MATERIAL);
    smatch match;
    regex_search(line, match, r1);

    std::string m_name = match[1];
    meshes.back().mat_name = m_name;
    
}

void Obj_Reader::obj_face_handler(std::string line)
{
    regex r0(OBJ_FACE_0);

    smatch match;
    
    if(regex_match(line, match, r0))
    {
        // 3 indices per face.
        // Only vertices x 3
        meshes.back().faces.push_back(Obj_Face(stoi(match[1]) -1, -1, -1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[2]) -1, -1, -1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[3]) -1, -1, -1));
        return;
    }
    // remember to subtract 1 from each indice as Wavefront obj files start at 0
    
    regex r1(OBJ_FACE_1);
    
    if(regex_match(line, match, r1))
    {
        // 4 indices per face.
        // Only vertices x 4
        meshes.back().faces.push_back(Obj_Face(stoi(match[1]) -1, -1, -1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[2]) -1, -1, -1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[3]) -1, -1, -1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[4]) -1, -1, -1));
        return;
    }
    
    regex r2(OBJ_FACE_2);
    
    if(regex_match(line, match, r2))
    {
        // vertex/texture/normal x 3
        meshes.back().faces.push_back(Obj_Face(stoi(match[1]) - 1, stoi(match[2]) - 1, stoi(match[3]) - 1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[4]) - 1, stoi(match[5]) - 1, stoi(match[6]) - 1));
        meshes.back().faces.push_back(Obj_Face(stoi(match[7]) - 1, stoi(match[8]) - 1, stoi(match[9]) - 1));
        return;
    }

	regex r3(OBJ_FACE_3);

	if(regex_match(line, match, r3))
	{
		// vertex//normal x 3
		meshes.back().faces.push_back(Obj_Face(stoi(match[1]) - 1, -1, stoi(match[2]) - 1));
		meshes.back().faces.push_back(Obj_Face(stoi(match[3]) - 1, -1, stoi(match[4]) - 1));
		meshes.back().faces.push_back(Obj_Face(stoi(match[5]) - 1, -1, stoi(match[6]) - 1));
		return;
	}
    
    regex r4(OBJ_FACE_4);
    
	if(regex_match(line, match, r4))
	{
		// vertex//normal x 3
		meshes.back().faces.push_back(Obj_Face(stoi(match[1]) - 1, stoi(match[2]) - 1, -1));
		meshes.back().faces.push_back(Obj_Face(stoi(match[3]) - 1, stoi(match[4]) - 1, -1));
		meshes.back().faces.push_back(Obj_Face(stoi(match[5]) - 1, stoi(match[6]) - 1, -1));
		return;
	}
}


Obj::Obj()
{
    
}

Obj::Obj(string material_name, string obj_name)
{
    load_obj(material_name, obj_name);
}

string Obj::get_path(std::string file_name)
{
    std::string file_path;
/*#ifdef __APPLE__
    int pos = file_name.find('.');
    if(pos == string::npos)
        throw (runtime_error("No file extension in: " + file_name));
    
    string name(begin(file_name), begin(file_name) + pos++);
    string extension(begin(file_name) + pos, end(file_name));
    
    NSString *file_name_ = [[NSBundle mainBundle] pathForResource:[NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding] ofType:[NSString stringWithCString:extension.c_str() encoding:NSUTF8StringEncoding]];
    
    if(nil == file_name_)
        throw (runtime_error("Unable to find path for: " + file_name));
    
    file_path = file_name_.UTF8String;
    
#else   */                
    file_path = file_name;  // Windows machine ?
    
    return file_path;
}

void Obj::make_obj(string mat_path, string obj_path)
{
    model_data.reset(new knu::Obj_Reader(mat_path, obj_path));
    
    if((!model_data->normals.empty()) && (!model_data->tex_coords.empty()))
    {
        // Vertex, normals and texture coords are present
        model_format = ObjFormat::Ver_Tex_Nor;
		model_format_size = sizeof(knu::math::Vector3f) + sizeof(knu::math::Vector3f) + sizeof(knu::math::Vector2f);
        for(auto &m : model_data->meshes)
        {
            meshes.push_back(Mesh());
            meshes.back().material = m.mat_name;
            
            for (auto &f : m.faces)
            {
                meshes.back().v.push_back(knu::math::Vector3f());
                meshes.back().t.push_back(knu::math::Vector2f());
                meshes.back().n.push_back(knu::math::Vector3f());
                meshes.back().v.back() = knu::math::Vector3f(model_data->vertices[f.v_index].x, model_data->vertices[f.v_index].y,
                                                                  model_data->vertices[f.v_index].z);
                meshes.back().t.back() = knu::math::Vector2f(model_data->tex_coords[f.t_index].x, model_data->tex_coords[f.t_index].y);
                meshes.back().n.back() = knu::math::Vector3f(model_data->normals[f.n_index].x, model_data->normals[f.n_index].y,
                                                                  model_data->normals[f.n_index].z);
            }
            
        }
        
    }else if (model_data->normals.empty() && (!model_data->tex_coords.empty()))
    {
        // tex coords are present, but no normals
        model_format = ObjFormat::Ver_Tex;
		model_format_size = sizeof(knu::math::Vector3f) + sizeof(knu::math::Vector2f);
        for(auto &m : model_data->meshes)
        {
            meshes.push_back(Mesh());
            meshes.back().material = m.mat_name;
            
            for (auto &f : m.faces)
            {
                meshes.back().v.push_back(knu::math::Vector3f());
                meshes.back().t.push_back(knu::math::Vector2f());
                
                meshes.back().v.back() = knu::math::Vector3f(model_data->vertices[f.v_index].x, model_data->vertices[f.v_index].y,
                                                             model_data->vertices[f.v_index].z);
                meshes.back().t.back() = knu::math::Vector2f(model_data->tex_coords[f.t_index].x, model_data->tex_coords[f.t_index].y);
            }
            
        }

        
    }else if ((!model_data->normals.empty()) && model_data->tex_coords.empty())
    {
        // normals are present, but no tex coords
        model_format = ObjFormat::Ver_Nor;
		model_format_size = sizeof(knu::math::Vector3f) + sizeof(knu::math::Vector3f);
        
        for(auto &m : model_data->meshes)
        {
            meshes.push_back(Mesh());
            meshes.back().material = m.mat_name;
            
            for (auto &f : m.faces)
            {
                meshes.back().v.push_back(knu::math::Vector3f());
                meshes.back().n.push_back(knu::math::Vector3f());
                
                meshes.back().v.back() = knu::math::Vector3f(model_data->vertices[f.v_index].x, model_data->vertices[f.v_index].y,
                                                             model_data->vertices[f.v_index].z);
                meshes.back().n.back() = knu::math::Vector3f(model_data->normals[f.n_index].x, model_data->normals[f.n_index].y,
                                                             model_data->normals[f.n_index].z);
            }
        }

        
    }else if (model_data->normals.empty() && model_data->tex_coords.empty())
    {
        // no normals or tex coords
        model_format = ObjFormat::Ver;
        model_format_size = sizeof(knu::math::Vector3f);

        for(auto &m : model_data->meshes)
        {
            meshes.push_back(Mesh());
            meshes.back().material = m.mat_name;
            
            meshes.back().v.reserve(m.faces.size());
            for (auto &f : m.faces)
            {
                meshes.back().v.push_back(knu::math::Vector3f());
                meshes.back().v.back() = knu::math::Vector3f(model_data->vertices[f.v_index].x, model_data->vertices[f.v_index].y,
                                                             model_data->vertices[f.v_index].z);
            }
        }

    }
}

void Obj::make_mat()
{
    for (auto &m : model_data->materials)
    {
		str_mat_map[m.mat_name] = m;
    }
}

void Obj::load_obj(std::string material_name, std::string obj_name)
{
    auto mat_path = get_path(material_name);
    auto obj_path = get_path(obj_name);
    make_obj(mat_path, obj_path);
    make_mat();
	model_data.reset();
}

#ifndef DO_NOT_INCLUDE_MODEL_PORTION
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#ifdef WIN32
#include <GL/glew.h>
#endif

Model_Obj::Model_Obj():
modelBuffer(0), modelVao(0), verticesCount(0), vSize(0), nSize(0), tSize(0)
{
    
}

Model_Obj::Model_Obj(std::string modelName, std::string pathOfTextures)
{
    load_model(modelName, pathOfTextures);
}

Model_Obj::~Model_Obj()
{
    destory_model();
}

void Model_Obj::load_model(std::string modelName, std::string pathOTextures)
{
    Obj o(modelName + ".mtl", modelName + ".obj");
    retrieve_vertices_information(o);
    fill_buffer(o);
    setup_vao(o);
    setup_materials(o);
}

void Model_Obj::retrieve_vertices_information(const knu::Obj &o)
{
    if(o.model_format == ObjFormat::Ver)
    {
        // only vertex information
        verticesCount = o.get_vertex_data().size();
        vertexInfo = VertexInfo::vertex;
        nSize = sizeof(knu::math::Vector3f) * verticesCount;
        bufferSize = nSize;
        
    }else
        if(o.model_format == ObjFormat::Ver_Nor)
        {
            // model specifies vertex normal
            verticesCount = o.get_vertex_data().size();
            vertexInfo = VertexInfo::vertex_normal;
            vSize = verticesCount * sizeof(knu::math::Vector3f);
            nSize = vSize;
            bufferSize = vSize + nSize;
            
        }
        else
            if(o.model_format == ObjFormat::Ver_Tex)
            {
                // model specifies vertex texcoord information
                verticesCount = o.get_vertex_data().size();
                vertexInfo = VertexInfo::vertex_texture;
                vSize = verticesCount * sizeof(knu::math::Vector3f);
                tSize = verticesCount * sizeof(knu::math::Vector2f);
                bufferSize = vSize + tSize;
                
                
            }
            else
                if(o.model_format == ObjFormat::Ver_Tex_Nor)
                {
                    // model specifies vertex texcoord normal information
                    verticesCount = o.get_vertex_data().size();
                    vertexInfo = VertexInfo::vertex_texture_normal;
                    vSize = verticesCount * sizeof(knu::math::Vector3f);
                    tSize = verticesCount * sizeof(knu::math::Vector2f);
                    nSize = vSize;
                    bufferSize = vSize + tSize + nSize;
                    
                }
                else
                {
                    // how did we get here?
                    throw std::runtime_error("retrieve_vertices_count() - model file has no vertex, texcoord, or normal information");
                }
}

void Model_Obj::fill_buffer(const knu::Obj &o)
{
    
    if(o.model_format == ObjFormat::Ver)
    {
        // only vertex information
        glGenBuffers(1, &modelBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
        std::vector<knu::math::Vector3f> v = o.get_vertex_data();
        vSize = sizeof(knu::math::Vector3f) * v.size();
        glBufferData(GL_ARRAY_BUFFER, vSize, v.data(), GL_STATIC_DRAW);
    }
    
    if(o.model_format == ObjFormat::Ver_Nor)
    {
        glGenBuffers(1, &modelBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
        std::vector<knu::math::Vector3f> v = o.get_vertex_data();
        std::vector<knu::math::Vector3f> n = o.get_normal_data();
        vSize = sizeof(knu::math::Vector3f) * v.size();
        nSize = sizeof(knu::math::Vector3f) * n.size();
        glBufferData(GL_ARRAY_BUFFER, vSize + nSize, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, v.data());
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, n.data());
    }
    
    if(o.model_format == ObjFormat::Ver_Tex)
    {
        glGenBuffers(1, &modelBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
        std::vector<knu::math::Vector3f> v = o.get_vertex_data();
        std::vector<knu::math::Vector2f> t = o.get_tex_coord_data();
        vSize = sizeof(knu::math::Vector3f) * v.size();
        tSize = sizeof(knu::math::Vector2f) * t.size();
        glBufferData(GL_ARRAY_BUFFER, vSize + tSize, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, v.data());
        glBufferSubData(GL_ARRAY_BUFFER, vSize, tSize, t.data());
    }
    
    if(o.model_format == ObjFormat::Ver_Tex_Nor)
    {
        glGenBuffers(1, &modelBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
        std::vector<knu::math::Vector3f> v = o.get_vertex_data();
        std::vector<knu::math::Vector2f> t = o.get_tex_coord_data();
        std::vector<knu::math::Vector3f> n = o.get_normal_data();
        vSize = sizeof(knu::math::Vector3f) * v.size();
        tSize = sizeof(knu::math::Vector2f) * t.size();
        nSize = sizeof(knu::math::Vector3f) * n.size();
        glBufferData(GL_ARRAY_BUFFER, vSize + tSize + nSize, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, v.data());
        glBufferSubData(GL_ARRAY_BUFFER, vSize, tSize, t.data());
        glBufferSubData(GL_ARRAY_BUFFER, vSize + tSize, nSize, n.data());
    }
}

void Model_Obj::setup_vao(const knu::Obj &o)
{
    glGenVertexArrays(1, &modelVao);
    glBindVertexArray(modelVao);
    glBindBuffer(GL_ARRAY_BUFFER, modelBuffer);
    
    if(o.model_format == ObjFormat::Ver)
    {
        // only vertex information
        glVertexAttribPointer((int)AttributeLocations::posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray((int)AttributeLocations::posAttrib);

    }else
        if(o.model_format == ObjFormat::Ver_Nor)
        {
            // model specifies vertex normal
            glVertexAttribPointer((int)AttributeLocations::posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
            glVertexAttribPointer((int)AttributeLocations::normAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)vSize);
            glEnableVertexAttribArray((int)AttributeLocations::posAttrib);
            glEnableVertexAttribArray((int)AttributeLocations::normAttrib);
        }
        else
            if(o.model_format == ObjFormat::Ver_Tex)
            {
                // model specifies vertex texcoord information
                glVertexAttribPointer((int)AttributeLocations::posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                glVertexAttribPointer((int)AttributeLocations::texcAttrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)vSize);
                glEnableVertexAttribArray((int)AttributeLocations::posAttrib);
                glEnableVertexAttribArray((int)AttributeLocations::texcAttrib);

            }
            else
                if(o.model_format == ObjFormat::Ver_Tex_Nor)
                {
                    // model specifies vertex texcoord normal information
                    glVertexAttribPointer((int)AttributeLocations::posAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
                    glVertexAttribPointer((int)AttributeLocations::texcAttrib, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)vSize);
                    glVertexAttribPointer((int)AttributeLocations::normAttrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vSize + tSize));
                    glEnableVertexAttribArray((int)AttributeLocations::posAttrib);
                    glEnableVertexAttribArray((int)AttributeLocations::texcAttrib);
                    glEnableVertexAttribArray((int)AttributeLocations::normAttrib);
  
                }
                else
                {
                    // how did we get here?
                    glBindVertexArray(0);
                    throw std::runtime_error("setup_vao() - model file has no vertex, texcoord, or normal information");
                }
    
    glBindVertexArray(0);
}

void Model_Obj::setup_materials(const knu::Obj &o)
{
    
    str_mat_map = o.str_mat_map;
    meshMaterialName = o.meshes.front().material;
    auto it = str_mat_map.find(meshMaterialName);
    meshMaterial = it->second;
    
    
}
void Model_Obj::destory_model()
{
    if(modelBuffer)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &modelBuffer);
    }
    
    if(modelVao)
    {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &modelVao);
    }
}

Obj_Material Model_Obj::get_obj_material() const
{
    return meshMaterial;
}

size_t Model_Obj::get_vertices_count() const
{
    return verticesCount;
}

void Model_Obj::draw()
{
    glBindVertexArray(modelVao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesCount);
    glBindVertexArray(0);
}
#endif  // DO_NOT_INCLUDE_MODEL_PORTION

