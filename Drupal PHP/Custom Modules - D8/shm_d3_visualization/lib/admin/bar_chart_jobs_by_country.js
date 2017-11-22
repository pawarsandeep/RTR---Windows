(function() {
var margin = {top: 20, right: 20, bottom: 50, left: 40}, 
width = 500,
height = 250;
width = width - margin.left - margin.right;
height = height - margin.top - margin.bottom;
var svg = d3.select("#bar-chart-jobs-by-country-admin")
	.append("svg")
	.attr("width", width - margin.right - margin.left-40)
	.attr("height", height + margin.top + margin.bottom)
	.append("g")
	.attr("transform", "translate(" + margin.left + " , " + margin.top + ")");
var x0 = d3.scaleBand().rangeRound([0, width], .1).padding(.1),
	x1 = d3.scaleBand(),
	y = d3.scaleLinear().range([height, 0]);

var color = d3.scaleOrdinal(d3.schemeCategory10);
	
d3.json("/rest-export/admin-jobs-by-country", function(error, data) {
	if(error) throw error;
	var mon = ['Jan','Feb','Mar','Apr','may','June','july','Aug','Sept','Oct','Nov','Dec'];
	var status = d3.keys(data[0]).filter(function(key) { return key !== "Country"; });
	data.forEach(function(d) {
	    d.counts = status.map(function(stat) { return {stat: stat, value: +d[stat]}; });
	});
	x0.domain(data.map(function(d) { return d.Country; }));
	x1.domain(status).range([0, x0.bandwidth()]);
	y.domain([0, d3.max(data, function(d) { return d3.max(d.counts, function(d) { return d.value; }); })]);
	svg.append("g")
		.attr("class", "axis axis--x")
		.attr("transform", "translate(0," + height + ")")
		.call(d3.axisBottom(x0));
		
	svg.append("g")
		.attr("class", "axis axis--y")
		.call(d3.axisLeft(y))
	.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 6)
		.attr("dy", "0.71em")
		.attr("text-anchor", "end")
		.text("Count");
	var stat = svg.selectAll(".country")
    	.data(data)
    	.enter().append("g")
    	.attr("class", "country")
    	.attr("transform", function(d) { 
    		var a = x0(d.Country);
    		return "translate(" + x0(d.Country) + ",0)"; 
    		});
	
	stat.selectAll("rect")
    .data(function(d) { return d.counts; })
    .enter().append("rect")
    .attr("width", x1.bandwidth())
    .attr("x", function(d) { 
    	return x1(d.stat);
    	})
    .attr("y", height)
    .attr("height", 0)
    .style("fill", function(d,i) { return color(i); })
    .transition()
    .duration(1500)
    .attr("y", function(d) { return y(d.value); })
    .attr("height", function(d) { return height - y(d.value); });
	
	var legend = svg.selectAll(".legend")
	.data(data)
	.enter().append("g")
	.attr("class", "legend")
	.attr("transform", function(d, i) { return "translate(" + i * 120 + "," + (height+35) + ")"; });

	legend.append("rect")
		.attr("height", 10)
		.attr("width", 10)
		.attr("fill", function(d, i) {return color(i)});
	legend.append("text")
		.text(function(d,i){return status[i]})
		.attr("transform","translate(15,10)");
});


}());