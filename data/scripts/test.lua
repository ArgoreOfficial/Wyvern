
function update()
	local tfm = entity.getTransform()
	
	tfm.position.x = math.sin(game.time() * 4.0)
	tfm.position.y = math.cos(game.time() * 7.0)
	tfm.position.z = math.cos(game.time() * 4.0)
end