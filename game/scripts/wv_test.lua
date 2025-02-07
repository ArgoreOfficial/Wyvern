print( "Normal lua print" )

function sum( _a, _b ) 
    wv.logDebug( "Sum was called with " .. tostring(_a) .. " and " .. tostring( _b ) )
    return _a + _b; 
end