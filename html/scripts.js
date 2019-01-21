const webhook_url = 'https://triggers.losant.com/webhooks/fblwP_2lx_cfklW-1KPCAzmCdlvQ3VhF1OQJyz1PaA1$';


function keyPress(key){


	var post_data = new Object();
	post_data["key_id"] = key;
	console.log(JSON.stringify(post_data));

	fetch(webhook_url, {
  		method: 'POST',
		headers: {
    		'Accept': 'application/json, text/plain, */*',
		    'Content-Type': 'application/json'
  		},
  		body: JSON.stringify(post_data)
	})
	.then(res=>res.json())
  	.then(res => console.log(res));

}