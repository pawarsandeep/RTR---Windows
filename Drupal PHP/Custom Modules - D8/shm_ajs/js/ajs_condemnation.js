

var app = angular.module('CondemnationJob',['shmAjs','autoCompleteModule','ngDialog']);
var a = 10;
app.config(function($interpolateProvider, $locationProvider){
    $interpolateProvider.startSymbol('[[').endSymbol(']]');
    $locationProvider.html5Mode(true).hashPrefix('!');
});

app.filter('byDeleteFlag', function () {
   return function (gasCylinders, filter) {
	    if(gasCylinders)
        {
       var validCylinders = [];

       for (i=0; i < gasCylinders.length; i++)
       {
            if(gasCylinders[i].delete_flag != filter)
            {
                validCylinders.push(gasCylinders[i]);
            }
       }
       return validCylinders;
		}
   }
});

app.controller('condemnationForm', ['$scope', 'CondemnationJob', 'liferaft', 'ngDialog','vessel','$location','editCondemnation','$window','serviceengineer',  function($scope,CondemnationJob,liferaft,ngDialog,vessel,$location,editCondemnation,$window,serviceengineer){
    $scope.condemnation_job = {};
    $scope.gas_cylinder = {};
    $scope.condemnation_job.liferaftNew = {};
    $scope.condemnation_job.liferaft = {};
    $scope.showSuccess = false;
    $scope.liferaftNew={};
    $scope.result = {};
	$scope.condemnation_job.field_service_engineer ={};
	$scope.condemnation_job.moderation_state = '';
   // $scope.condemnation_job.gas_cylinders.delete_flag = {};
    $scope.condemnation_job.gas_cylinders=[{cylinder_sn:"",
        lpt:"",
        ptest_req:"",
        refill_req:"",
        full_weight:"",
        tare_weight:"",
        gas_weight_n2:"",
        gas_weight_co2:"",
        actual_full_weight:"",
        delete_flag:"",
    }];


    $scope.condemnationAutocomplete = null;


    $scope.addGasCylinderCondemnation = function(){
        $scope.condemnation_job.gas_cylinders.push({cylinder_sn:"", delete_flag:false});
    };
    $scope.condemnation_job.gas_cylinders.delete_flag = false;
    $scope.removeGasCylinderCondemnation = function(gas_cylinder){

        for (var i=0;i<$scope.condemnation_job.gas_cylinders.length;i++){

            if($scope.condemnation_job.gas_cylinders[i]===gas_cylinder){
               // $scope.condemnation_job.gas_cylinders.splice(i,1);
                $scope.condemnation_job.gas_cylinders[i].delete_flag = true;
            }
        }
    };
    $scope.isDisabled = true;

    $scope.getMatchedCondemnation ={
        minimumChars: 2,
        data: function (searchText) {
            var type = 'liferaft';
            return liferaft.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-details-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            var dataPromise = liferaft.getDetails(item.item.nid);
            dataPromise.then(function(result) {
                if(result.status == 200)
                {
                    $scope.condemnation_job = result;
                }

                var a = 10;

            });

        }

    };

    $scope.getMatchedLiferaftVessel ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText,'lr-vessel');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'vessel-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            $scope.liferaftNew.field_liferaft_name_vessel = item;
        },
    };

    $scope.getMatchedLiferaftManufacturer ={
        minimumChars: 2,
        data: function (searchText) {
            var type = 'lr-manufacturer';
            return liferaft.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-manufacturer-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function (item) {
            $scope.condemnation_job.liferaftNew.field_liferaft_manufacturer = item.item;
        }
    };

    $scope.getMatchedLiferaftType = {
        minimumChars: 2,
        data: function (searchText) {
            var type = 'liferaft-type';
            return liferaft.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.name | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-type-autocomplete',
        selectedTextAttr: 'name',
        itemSelected: function (item) {
            $scope.condemnation_job.liferaftNew.field_liferaft_type = item.item;
        }
    }

    $scope.AddLiferaftJob = function () {

        ngDialog.open(
            {
                template: 'CondemnationLiferaftNewJob',
                className: 'ngdialog-theme-default',
                controller: 'dialogCtrl',
                scope: $scope,
                closeByDocument: false,
            }
        );
    }

    var path = $location.path();

    if(path.match(/^\/condemnation\/\d+\/edit$/))
    {
        var values = path.split("/");
        var entity_id = values[2];
        var promise = editCondemnation.getDetails(entity_id);
        promise.then(function (result) {
            Object.assign($scope.condemnation_job, result);
            var a=10;
        });

    }

    var path_new_job = $location.url();
    if(path_new_job.match(/^\/condemnation\/new-job\?lr_id\=\d+$/))
    {
        var query_str =$location.search();
        var lr_id_query = query_str.lr_id;
        var dataPromise = liferaft.getDetails(lr_id_query);
        dataPromise.then(function(result) {
            if(result.status == 200){
                $scope.condemnation_job = result;
            }

            var a = 10;

        });
    }



    $scope.saveJobCondemnation = function(){
        var path = $location.path();

        if(path.match(/^\/condemnation\/\d+\/edit$/)) {
            //alert('save clicked');
            $scope.condemnation_job.modeartion_state = 'approved';
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
            var values = path.split("/");
            var entity_id = values[2];
            var promise =  editCondemnation.updateCodenmnationJob($scope.condemnation_job,entity_id);
            promise.then(function (result) {
                $window.location.href ='/service/'+entity_id;
            });
            }
            else
            {
                $scope.condemnation_job.showError = true;
            }
        }
        else {
            console.log(JSON.stringify($scope.condemnation_job));
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
            var promise = CondemnationJob.createcondemnationJob($scope.condemnation_job);
            promise.then(function (result) {

                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href ='/service/'+entity_id;
            });
            $scope.condemnation_job.showSuccess = true;
            }
            else
            {
                $scope.condemnation_job.showError = true;
            }
        }
    };

    $scope.draftJob = function () {

        var path = $location.path();

        if(path.match(/^\/condemnation\/\d+\/edit$/)) {
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
                $scope.condemnation_job.modeartion_state = 'draft';
                alert('save clicked');
                var values = path.split("/");
                var entity_id = values[2];
                var promise =  editCondemnation.updateCodenmnationJob($scope.condemnation_job,entity_id);
                promise.then(function (result) {
                    $window.location.href = '/service/' + entity_id;
                });
            }
            else
                {
                    $scope.condemnation_job.showError = true;
                }
        }
        else {
            console.log(JSON.stringify($scope.condemnation_job));
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
            $scope.condemnation_job.modeartion_state = 'draft';
                var promise = CondemnationJob.createcondemnationJob($scope.condemnation_job);
            promise.then(function (result) {
                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href ='/service/'+entity_id;
            });
            $scope.condemnation_job.showSuccess = true;
            }
            else
            {
                $scope.condemnation_job.showError = true;
            }
        }

    }
    $scope.sendForAprrovalJob = function () {

        var path = $location.path();

        if (path.match(/^\/condemnation\/\d+\/edit$/)) {
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
            $scope.condemnation_job.modeartion_state = 'pending_approval';
            alert('save clicked');
            var values = path.split("/");
            var entity_id = values[2];
                var promise =  editCondemnation.updateCodenmnationJob($scope.condemnation_job,entity_id);
            promise.then(function (result) {
                $window.location.href = '/service/' + entity_id;
            });
            }
            else
            {
                $scope.condemnation_job.showError = true;
            }
        }
        else {
            console.log(JSON.stringify($scope.condemnation_job));
            if($scope.condemnation_job.liferaft.field_liferaft_serial_no) {
            $scope.condemnation_job.modeartion_state = 'pending_approval';
                var promise = CondemnationJob.createcondemnationJob($scope.condemnation_job);
            promise.then(function (result) {
                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href = '/service/' + entity_id;
            });
            $scope.condemnation_job.showSuccess = true;
            }
            else
            {
                $scope.condemnation_job.showError = true;
            }
        }
    }
    $scope.approveJob = function () {
        var path = $location.path();

        if(path.match(/^\/condemnation\/\d+\/edit$/)) {
            $scope.condemnation_job.modeartion_state = 'approved';
            alert('save clicked');
            var values = path.split("/");
            var entity_id = values[2];
            var promise =  editCondemnation.updateCodenmnationJob($scope.condemnation_job,entity_id);
            promise.then(function (result) {
                $window.location.href ='/service/'+entity_id;
            });
        }
    }
    $scope.rejectedJob = function () {
        var path = $location.path();

        if(path.match(/^\/condemnation\/\d+\/edit$/)) {
            $scope.condemnation_job.modeartion_state = 'draft';
            alert('save clicked');
            var values = path.split("/");
            var entity_id = values[2];
            var promise =  editCondemnation.updateCodenmnationJob($scope.condemnation_job,entity_id);
            promise.then(function (result) {
                $window.location.href ='/service/'+entity_id;
            });
        }
    }

	 $scope.getMatchedServiceEngineer ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText, 'service-engineer');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.name | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-autocomplete',
        selectedTextAttr: 'name',
        itemSelected: function(item) {
            $scope.condemnation_job.field_service_engineer = item.item;

        },
    };



}]);



app.controller('dialogCtrl', ['$scope', 'ngDialog','autocompleteProvider', 'vessel', function($scope, ngDialog,autocompleteProvider, vessel){
    //$scope = $scope.$parent;
    $scope.liferaftNew = {};
    $scope.liferaftNew.vessel = {};
    $scope.condemnationAutocomplete = null;
    $scope.addNewLiferaft = function(){
        $scope.$parent.condemnation_job.liferaft = $scope.liferaftNew;
        $scope.$parent.condemnation_job.liferaft.isNew = true;
        ngDialog.close();
    };

    $scope.getMatchedLiferaftVessel ={
        minimumChars: 2,
        data: function (searchText) {
            var type = 'lr-vessel';
            return autocompleteProvider.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-vessel-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function (item) {
            $scope.liferaftNew.vessel = item.item;
            var promise = vessel.getDetails($scope.liferaftNew.vessel.nid);
            promise.then(function (result) {
                Object.assign($scope.liferaftNew.vessel, result);
                var a=10;
            });
        }

    };
    $scope.getMatchedLiferaftType = {
        minimumChars: 2,
        data: function (searchText) {
            var type = 'liferaft-type';
            return autocompleteProvider.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.type_capacity | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-type-autocomplete',
        selectedTextAttr: 'type_capacity',
        itemSelected: function (item) {
            $scope.liferaftNew.field_liferaft_type = item.item;
        }
    };
}]);