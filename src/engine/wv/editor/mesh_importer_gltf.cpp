#include "mesh_importer_gltf.h"

#include <wv/application.h>

#include <wv/filesystem/file_system.h>

#include <wv/rendering/mesh.h>
#include <wv/rendering/material.h>
#include <wv/rendering/texture.h>

#include <wv/shaders/lit_shader.h>

#include <wv/thread/task_system.h>
#include <wv/debug/timer.h>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

template <>
struct fastgltf::ElementTraits<wv::Vector3f> : fastgltf::ElementTraitsBase<wv::Vector3f, AccessorType::Vec3, float> { };

template <>
struct fastgltf::ElementTraits<wv::Vector2f> : fastgltf::ElementTraitsBase<wv::Vector2f, AccessorType::Vec2, float> { };

void wv::MeshImporterGLTF::load( const std::filesystem::path& _path, MeshImportOptions _options )
{
	m_loaded = false;

	// Load file
	IFileSystem* fs = Application::getSingleton()->getFileSystem();

	constexpr auto gltfOptions = 
		  fastgltf::Options::DontRequireValidAssetMember 
		| fastgltf::Options::AllowDouble 
		| fastgltf::Options::LoadGLBBuffers 
		| fastgltf::Options::LoadExternalBuffers;

	fastgltf::Parser parser;
	std::filesystem::path fullpath = fs->getFullPath( _path );

	fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath( fullpath );
	if ( data.error() != fastgltf::Error::None )
	{
		WV_LOG_ERROR( "Failed to load model %s\n", _path.string().c_str() );
		return;
	}

	fastgltf::Expected<fastgltf::Asset> load = parser.loadGltf( data.get(), fullpath.parent_path(), gltfOptions );
	if ( load.error() != fastgltf::Error::None )
	{
		WV_LOG_ERROR( "Failed to parse model%s\n", _path.string().c_str() );
		return;
	}

	fastgltf::Asset& asset = load.get();

	std::vector<Ref<TextureAsset>> textures;
	textures.resize( asset.images.size() );

	TaskSystem* taskSystem = Application::getSingleton()->getTaskSystem();
	ThreadWorker* worker = taskSystem->getThreadWorker();
	Fence* loadFence = taskSystem->allocateFence();

	if ( m_meshManager->contains( _path ) )
	{
		m_meshAsset = m_meshManager->get( _path );
	}
	else
	{
		m_meshAsset = std::make_shared<MeshAsset>();
		m_meshManager->add( _path, m_meshAsset );

		worker->push( loadFence, [ this, _path, fullpath, _options, &asset ]( auto, auto ) {
			parseMesh( asset, _options );
			
			// serialize
			m_meshAsset->m_path = _path;
			m_meshAsset->serialize( fullpath );
		} );
	}

	for ( size_t i = 0; i < asset.images.size(); i++ )
	{
		fastgltf::Image& image = asset.images[ i ];
		
		if ( m_textureManager->contains( image.name ) )
			textures[ i ] = m_textureManager->get( image.name );
		else
		{
			worker->push( loadFence, [ this, _path, i, &image, &textures, &asset ]( auto, auto ) {
				std::visit( fastgltf::visitor{
					[]( auto& arg ) { },
					[ & ]( fastgltf::sources::URI& _filePath ) {
						if ( _filePath.fileByteOffset == 0 && _filePath.uri.isLocalPath() )
							textures[ i ] = m_textureManager->get( _path.parent_path() / _filePath.uri.fspath() );
					
					},
					[ & ]( fastgltf::sources::Vector& _vector ) {
						textures[ i ] = std::make_shared<TextureAsset>( (uint8_t*)_vector.bytes.data(), _vector.bytes.size() );
					
						// add texture to manager
						m_textureManager->add( image.name, textures[ i ] );
					},
					[ & ]( fastgltf::sources::BufferView& view ) {
						auto& bufferView = asset.bufferViews[ view.bufferViewIndex ];
						auto& buffer = asset.buffers[ bufferView.bufferIndex ];
					
						std::visit( fastgltf::visitor{
							[]( auto& arg ) { },
							[ & ]( fastgltf::sources::Array& _array ) {
								textures[ i ] = std::make_shared<TextureAsset>( (uint8_t*)( _array.bytes.data() + bufferView.byteOffset ), bufferView.byteLength );
							} },
							buffer.data );

						// add texture to manager
						m_textureManager->add( image.name, textures[ i ] );
					},
					},
					image.data );
			} );
		}
	}

	taskSystem->waitAndFreeFence( loadFence );

	const char* shaderName = "materials/default_lit.wvmt";

	for ( fastgltf::Material& mat : asset.materials )
	{
		Ref<MaterialAsset> material = m_materialManager->get( shaderName );
		LitShader::LitMaterialData materialData{};
		
		/*
		file.materials[ mat.name.c_str() ] = newMat;

		GLTFMetallic_Roughness::MaterialConstants constants;
		constants.colorFactors.x = mat.pbrData.baseColorFactor[ 0 ];
		constants.colorFactors.y = mat.pbrData.baseColorFactor[ 1 ];
		constants.colorFactors.z = mat.pbrData.baseColorFactor[ 2 ];
		constants.colorFactors.w = mat.pbrData.baseColorFactor[ 3 ];

		constants.metal_rough_factors.x = mat.pbrData.metallicFactor;
		constants.metal_rough_factors.y = mat.pbrData.roughnessFactor;
		// write material parameters to buffer
		sceneMaterialConstants[ data_index ] = constants;

		MaterialPass passType = MaterialPass::MainColor;
		if ( mat.alphaMode == fastgltf::AlphaMode::Blend )
		{
			passType = MaterialPass::Transparent;
		}

		GLTFMetallic_Roughness::MaterialResources materialResources;
		// default the material textures
		materialResources.colorImage = engine->_whiteImage;
		materialResources.colorSampler = engine->_defaultSamplerLinear;
		materialResources.metalRoughImage = engine->_whiteImage;
		materialResources.metalRoughSampler = engine->_defaultSamplerLinear;

		// set the uniform buffer for the material data
		materialResources.dataBuffer = file.materialDataBuffer.buffer;
		materialResources.dataBufferOffset = data_index * sizeof( GLTFMetallic_Roughness::MaterialConstants );
		// grab textures from gltf file
		*/
		if ( mat.pbrData.baseColorTexture.has_value() )
		{
			size_t img = asset.textures[ mat.pbrData.baseColorTexture.value().textureIndex ].imageIndex.value();
			// size_t sampler = asset.textures[ mat.pbrData.baseColorTexture.value().textureIndex ].samplerIndex.value();

			if ( textures[ img ] )
			{
				material->textureAssets.push_back( textures[ img ] );
				materialData.albedoIndex = textures[ img ]->getImageSlot();
			}
			else
			{
				materialData.albedoIndex = 0;
			}
		}
		else
		{
			// if there is not albedo texture, default to white (index 2), it will be multiplied later in the shader
			materialData.albedoIndex = 2;
		}

		materialData.albedoColor = wv::Vector4f(
			mat.pbrData.baseColorFactor.x(),
			mat.pbrData.baseColorFactor.y(),
			mat.pbrData.baseColorFactor.z(),
			mat.pbrData.baseColorFactor.w()
		);
		
		material->shaderType->setMaterialData( material->m_materialIndex, &materialData );

		m_materials.push_back( material );
	}

	m_loaded = true;
}

void wv::MeshImporterGLTF::parseMesh( fastgltf::Asset& _asset, MeshImportOptions _options )
{
	// Parse gltf

	GeometrySurface surface{};
	std::unordered_map<size_t, std::vector<size_t>> meshPrimitivesMap;

	for ( size_t meshIndex = 0; meshIndex < _asset.meshes.size(); meshIndex++ )
	{
		fastgltf::Mesh& mesh = _asset.meshes[ meshIndex ];
	
		for ( auto& primitive : mesh.primitives )
		{
			uint32_t indexOffset = surface.vertexCount();

			std::vector<uint32_t> indices;
			std::vector<Vector3f> positions;
			std::vector<Vector3f> normals;
			std::vector<Vector2f> uv0s;
			std::vector<Vector3f> colours;

			fastgltf::Attribute* posAttrib = primitive.findAttribute( "POSITION" );
			fastgltf::Attribute* norAttrib = primitive.findAttribute( "NORMAL" );
			fastgltf::Attribute* uv0Attrib = primitive.findAttribute( "TEXCOORD_0" );
			fastgltf::Attribute* colAttrib = primitive.findAttribute( "COLOR_0" );

			// load indices
			{
				fastgltf::Accessor& indexAccessor = _asset.accessors[ primitive.indicesAccessor.value() ];
				indices.resize( indexAccessor.count );
				fastgltf::copyFromAccessor<uint32_t>( _asset, indexAccessor, indices.data() );
			}

			// load vertex data
			{
				// vertex position
				if ( posAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& positionAccessor = _asset.accessors[ posAttrib->accessorIndex ];
					positions.resize( positionAccessor.count );
					fastgltf::copyFromAccessor<Vector3f>( _asset, positionAccessor, positions.data() );
				}

				// vertex normal
				if ( norAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& normalAccessor = _asset.accessors[ norAttrib->accessorIndex ];
					normals.resize( normalAccessor.count );
					fastgltf::copyFromAccessor<Vector3f>( _asset, normalAccessor, normals.data() );
				}

				// texture coordinate 0
				if ( uv0Attrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& texCoord0Accessor = _asset.accessors[ uv0Attrib->accessorIndex ];
					uv0s.resize( texCoord0Accessor.count );
					fastgltf::copyFromAccessor<Vector2f>( _asset, texCoord0Accessor, uv0s.data() );
				}

				// vertex colour 0
				//if ( colAttrib != primitive.attributes.end() )
				//{
				//	fastgltf::Accessor& colourAccessor = _asset.accessors[ colAttrib->accessorIndex ];
				//	colours.resize( colourAccessor.count );
				//	fastgltf::copyFromAccessor<Vector3f>( _asset, colourAccessor, colours.data() );
				//}
			}

			GeometrySurface::Primitive prim{};
			prim.firstIndex   = surface.indices.size();
			prim.vertexOffset = surface.vertexCount();
			prim.vertexCount  = positions.size();
			prim.indexCount   = indices.size();
			
			if ( primitive.materialIndex.has_value() )
				prim.material = *primitive.materialIndex;
			else
			{
				WV_LOG_WARNING( "%s does not have a material. Using default\n", mesh.name.c_str() );
				prim.material = 0;
			}

			meshPrimitivesMap[ meshIndex ].push_back( surface.primitives.size() );
			surface.primitives.push_back( prim );

			while ( normals.size() < positions.size() ) normals.push_back( { 0.0f, 1.0f, 0.0f } );
			while ( uv0s.size()    < positions.size() ) uv0s   .push_back( { 0.0f, 0.0f } );
			while ( colours.size() < positions.size() ) colours.push_back( { 1.0f, 1.0f, 1.0f } );

			surface.vertexPositions.insert( surface.vertexPositions.end(), positions.begin(), positions.end() );
			surface.vertexNormals.insert( surface.vertexNormals.end(), normals.begin(), normals.end() );
			surface.vertexUVs.insert( surface.vertexUVs.end(), uv0s.begin(), uv0s.end() );
			surface.vertexColours.insert( surface.vertexColours.end(), colours.begin(), colours.end() );

			surface.indices.insert( surface.indices.end(), indices.begin(), indices.end() );
		}
	}

	TaskSystem* taskSystem = Application::getSingleton()->getTaskSystem();
	ThreadWorker* worker = taskSystem->getThreadWorker();
	Fence* loadFence = taskSystem->allocateFence();
	
	fastgltf::iterateSceneNodes( _asset, 0, fastgltf::math::fmat4x4(),
		[ & ]( fastgltf::Node& _node, fastgltf::math::fmat4x4 _matrix ) {
		if ( !_node.meshIndex.has_value() )
			return;

		for ( size_t primitiveIndex : meshPrimitivesMap[ *_node.meshIndex ] )
		{
			worker->push( loadFence, [ _options, _matrix, primitiveIndex, &surface ]( auto, auto ) {
				wv::Matrix4x4f matrix( _matrix.data() );
				wv::Matrix3x3f normalMatrix = {};// transpose(inverse(mat3(modelMatrix)))
				normalMatrix.setRow( 0, { matrix.get( 0, 0 ), matrix.get( 0, 1 ), matrix.get( 0, 2 ) } );
				normalMatrix.setRow( 1, { matrix.get( 1, 0 ), matrix.get( 1, 1 ), matrix.get( 1, 2 ) } );
				normalMatrix.setRow( 2, { matrix.get( 2, 0 ), matrix.get( 2, 1 ), matrix.get( 2, 2 ) } );

				normalMatrix = wv::Math::transpose( wv::Math::inverse( normalMatrix ) );

				auto prim = surface.primitives[ primitiveIndex ];

				for ( size_t i = 0; i < prim.vertexCount; i++ )
				{
					auto& vec = surface.vertexPositions[ i + prim.vertexOffset ];
					vec = vec * matrix;
					vec = vec * _options.transform;
					
					auto& norm = surface.vertexNormals[ i + prim.vertexOffset ];
					norm = norm * normalMatrix;
					norm.normalize();
				}
			} );
		}		
	} );

	taskSystem->waitAndFreeFence( loadFence );

	m_meshAsset->initialize( surface );
}
