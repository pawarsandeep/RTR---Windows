(function() {
var margin = {top: 20, right: 20, bottom: 60, left: 60}, 
width = 280,
height = 250;
width = width - margin.left - margin.right;
height = height - margin.top - margin.bottom;
var svg = d3.select("#bar-chart-royalty-due-by-month-admin")
	.append("svg")
	.attr("width", width + margin.right + margin.left)
	.attr("height", height + margin.top + margin.bottom)
	.append("g")
	.attr("transform", "translate(" + margin.left + " , " + margin.top + ")");
var x = d3.scaleBand().rangeRound([0, width], .1).padding(".1"),
	z = d3.scaleOrdinal().range(["#5b9bd5", "#ed7d31"]),
	y = d3.scaleLinear().range([height, 0]);

var color = d3.scaleOrdinal()
	.range(["#98abc5", "#8a89a6", "#7b6888", "#6b486b", "#a05d56", "#d0743c", "#ff8c00"]);
var stack = d3.stack()
d3.json("/rest-export/admin-royalty-due-by-month", function(error, data) {
	if(error) throw error;
	data.sort(function(a,b) { return b.total - a.total; });
	var status = d3.keys(data[0]).filter(function(key) { return key !== "Month"; });
	data.forEach(function(d) {
		var tot = 0;
	    status.map(function(stat) { return {
			total: tot += +d[stat]};
		});
		d.total = tot;
	});
	x.domain(data.map(function(d) { return d.Month; }));
	z.domain(status);
	y.domain([0, d3.max(data, function(d) { 
	return d.total;
	})]).nice();
	svg.selectAll(".status")
		.data(stack.keys(status) (data))
		.enter().append("g")
		.attr("class", "serie")
		.attr("fill", function(d) {return z(d.key);})
		.selectAll("rect")
		.data(function(d) {return d;})
		.enter().append("rect")
		.attr("x", function(d) {
			return x(d.data.Month
			); })
		.attr("y", function(d) {return y(d[1]);})
		.attr("height",function(d) { return y(d[0]) - y(d[1]); })
		.attr("width", x.bandwidth())
		.on('mouseenter', function(d) {
			d3.select('#tooltip-admin-royalty-due-by-month-bar-chart')
			.classed('hidden',false)
			.style('position', 'absolute')
			.style("left",x(d.data.Month) + margin.left + (x.bandwidth() / 2) + 'px')
			.style("top", y(d.data.total) - 12 + 'px')
			.style('transform', 'translate(-50%,0)')
			.select('strong')
			.text('');
			d3.select('#tooltip-admin-royalty-due-by-month-bar-chart')
			.select('span')
			.text((d[1]-d[0]).toFixed(2).replace(/(\d)(?=(\d{3})+\.)/g, '$1,'));
		})
		.on('mouseleave', function(d) {
			d3.select('#tooltip-admin-royalty-due-by-month-bar-chart')
			.classed('hidden',true);
		});
	svg.append("g")
		.attr("class", "axis axis--x")
		.attr("transform", "translate(0," + height + ")")
		.call(d3.axisBottom(x))
		.selectAll("text")
		.attr("transform", "rotate(-45) translate(-30,0)");
		
	svg.append("g")
		.attr("class", "axis axis--y")
		.call(d3.axisLeft(y))
	.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 6)
		.attr("dy", "0.71em")
		.attr("text-anchor", "end")
		.text("Count");
	var stat = svg.selectAll(".Month")
    	.data(data)
    	.enter().append("g")
    	.attr("class", "Month")
    	.attr("transform", function(d) { 
    		var a = x(d.Month);
    		return "translate(" + x(d.Month) + ",0)"; 
    		});
	
		var legend = svg.selectAll(".legend")
	.data(data)
	.enter().append("g")
	.attr("class", "legend")
	.attr("transform", function(d, i) { return "translate(" + i * 120 + "," + (height+50) + ")"; });
	legend.append("rect")
	.attr("height", 10)
	.attr("width", 10)
	.attr("fill", function(d, i) {return z(i)});
	legend.append("text")
	.text(function(d,i){return status[i]})
	.attr("transform","translate(15,10)");
	
	stat.selectAll("rect")
    .data(function(d) { return d.counts; })
    .enter().append("rect")
	.attr("width", x.bandwidth())
    .attr("x", function(d) { 
    	return x(d.stat);
    	})
	.attr("height",0)
	.attr("y",height)
    .style("fill", function(d) { return color(d.stat); })
	.transition()
	.duration(1500)
    .attr("height", function(d) { return height - y(d.value); })
	.attr("y", function(d) { return y(d.value); });
    
    
	/*g.selectAll(".bar")
		.data(data)
		.enter().append("rect")
		.attr("class", "bar")
		.attr("x", function(d) { return x(d.month); })
		.attr("y", function(d) { return y(d.count); })
		.attr("width", x.bandwidth())
		.attr("height", function(d) { return height - y(d.count); })
		.attr("fill", "steelblue");
*/
});
}());

