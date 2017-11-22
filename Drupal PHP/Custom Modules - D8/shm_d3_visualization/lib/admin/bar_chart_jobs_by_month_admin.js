(function() {
var width = 350,
height = 250,
margin = {top: 20, right: 20, bottom: 30, left: 40};
var svg = d3.select("#bar-chart-jobs-by-month-admin")
	.append("svg")
	.attr("width", width)
	.attr("height", height);
width = width - margin.left - margin.right;
height = height - margin.top - margin.bottom;
var x = d3.scaleBand().rangeRound([0, width]).padding(0.4),
	y = d3.scaleLinear().rangeRound([height,0]);
	
var g = svg.append("g")
	.attr("transform", "translate(" + margin.left + " , " + margin.top + ")");
	
d3.json("/rest-export/admin-jobs-by-month", function(error, data) {
	if(error) throw error;
	var mon = ['January','February','March','April','May','June','July','August','September','October','November','December'];
	data.forEach(function(d) {
		d.month = mon[d.month-1];
	});
	x.domain(data.map(function(d) { return d.month;}));
	y.domain([0, d3.max(data, function(d) {return +d.count;})]);

	g.append("g")
		.attr("class", "axis axis--x")
		.attr("transform", "translate(0," + height + ")")
		.call(d3.axisBottom(x));
		
	d3.selectAll("g.tick text")
		.attr("transform", "rotate(-30)")
		.attr("x", -10);
		
	g.append("g")
		.attr("class", "axis axis--y")
		.call(d3.axisLeft(y)
		.tickFormat(d3.format(".0f")));
		
		g.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 6)
		.attr("dy", "0.71em")
		.attr("text-anchor", "end")
		.text("Count");
		
	g.selectAll(".bar")
		.data(data)
		.enter().append("rect")
		.attr("class", "bar")
		.attr("x", function(d) { return x(d.month); })
		.attr("y", height)
		.attr("width", x.bandwidth())
		.attr("height", 0)
		.attr("fill", "steelblue")
		.transition()
		.duration(500)
		.attr("height", function(d) { return height - y(d.count); })
		.attr("y", function(d) { return y(d.count); })
		.delay(function(d,i) { return i*40;});
		g.selectAll(".bar").on('mouseenter', function(d) {
		d3.select('#tooltip-admin-jobs-by-month-bar-chart')
		.classed('hidden',false)
		.style('position', 'absolute')
		.style("left",x(d.month) + margin.left + (x.bandwidth() / 2) + 'px')
		.style("top", y(d.count) - 12 + 'px')
		.style('transform', 'translate(-50%,0)')
		.select('strong')
		.text(d.month + ': ');
		d3.select('#tooltip-admin-jobs-by-month-bar-chart')
		.select('span')
		.text(d.count);
	});
	g.selectAll(".bar").on('mouseleave', function(d) {
		d3.select('#tooltip-admin-jobs-by-month-bar-chart')
		.classed('hidden',true);
	});
});

}());